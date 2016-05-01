#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string> 
#include <iostream>	// console I/O
#include <fstream> 	// file I/O
#include <sstream>
#include <cstdlib> 
#include <netdb.h>
#include <iterator> 
#include "HTTP-message.h" 

int main(int argc, char **argv)
{
	// Parse command line args
	if (argc != 2){
		std::cerr<<"Usage: "<< argv[0] <<" <URL>\n";
		exit(1); 
	}

	// Create a HTTP Request
	std::string s(argv[1]); 
	HttpRequest request(s);

	int sockfd; // client socket  
	struct addrinfo hints, *server_info, *p;	
	int ret;	// check success/failure of socket API calls 

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
	hints.ai_socktype = SOCK_STREAM;

	std::map<std::string, std::string> headers = request.getHeaders(); 
	if ((ret = getaddrinfo(headers["Host"].c_str(), request.getPortNum().c_str(), &hints, &server_info)) != 0) {
	    std::cerr << "getaddrinfo: %s\n", gai_strerror(ret);
	    exit(1);
	}

	// loop through all the results and connect to the first we can
	for(p = server_info; p != NULL; p = p->ai_next) {
		
	    if ((sockfd = socket(p->ai_family, p->ai_socktype,
	            p->ai_protocol)) == -1) {
	        perror("socket");
	        continue;
	    }

	    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
	        perror("connect");
	        close(sockfd);
	        continue;
	    }

	    // reaching this point means we have connected successfully 
	    break; 
	}

	if (p == NULL) {
	    // looped off the end of the list with no connection
	    std::cerr << "Failed to connect\n"; 
	    exit(2);
	}

	// Send and receive data 
	ByteBlob response; 
	ByteBlob copy_of_request = request.encode();
	int size = copy_of_request.size();  
	ret = write(sockfd, copy_of_request.data(), size); 
	if (ret < 0){
		perror("socket write failed");
		exit(3); 
	}

	// Get the filename 
	std::string path = request.getUrl(); 
	std::stringstream ss(path); 
	std::string token, filename; 

	// Filename is after the last forward slash 
	while(std::getline(ss, token, '/')){
		filename = token; 
	}

	// Filename may be empty 
	// e.g. http://www.cnn.com/
	// If retrieval successful, save to default filename of index.html 
	if (filename.empty())
		filename = "index.html";

	// Read until \r\n\r\n is found
	// This is the end of HTTP response header
	// and start of HTTP response body (data) 
	size_t index = 0; 
	uint8_t buf[8192];
	memset(buf, 0, sizeof(buf)); 
	while ( (ret = read(sockfd, buf, sizeof(buf))) != 0) {
		if (ret < 0){
			perror("socket read failed"); 
			exit(4); 
		}
		response.insert(response.end(), buf, buf + ret);
		memset(buf, 0, sizeof(buf)); 

		// find \r\n\r\n
		while( ( (index + 3) < response.size() )  && (response[index] != '\r' || response[index + 1] != '\n' || response[index + 2] != '\r' || response[index + 3] != '\n') )
			index++;
		if (response[index] == '\r' && response[index + 1] == '\n' && response[index + 2] == '\r' && response[index + 3] == '\n'){
			break;
		}
	}
	
	// Save where body starts
	int body_index = index + 4; 
	// Parse reponse for HttpResponse Code
	index = 0; 
	while(response[index] != ' '){
		++index; 
	}
	++index; 
	std::string status_code; 
	while(response[index] != ' '){
		status_code += response[index++]; 
	}

	std::cout << "HTTP request sent, awaiting response... "; 
	// Deal with status code 
	if(status_code == "200")
		std::cout << "200 OK\nSaving file to " << filename << "\n";
	else if (status_code == "400")
		std::cerr << "400 Bad Request\n";
	else if (status_code == "404")
		std::cerr << "404 Not Found\n";
	else if (status_code == "403")
		std::cerr << "403 Forbidden\n";
	else if (status_code == "501")
		std::cerr << "501 Not Implemented\n"; 
	else if (status_code == "505")
		std::cerr << "505 HTTP version not supported\n";
	else
		std::cerr << status_code << " error code encountered\n";

	// Exit upon unsuccessful retrieval 
	if(status_code != "200"){
		exit(1); 
	}

	// Copy leftover bytes from response body into file(do not copy over header bytes)
	std::ofstream outfile(filename, std::ios::out | std::ios::binary );
	std::ostream_iterator<uint8_t> oi(outfile, ""); 
	std::copy(response.begin() + body_index, response.end(), oi); 

	// Read the rest of the file 
	while ( (ret = read(sockfd, buf, sizeof(buf))) != 0) {
		// std::cout << ret << std::endl;
		if (ret < 0){
			perror("socket read failed"); 
			exit(4); 
		}
		ByteBlob segment(buf, buf + ret); 
		std::copy(segment.begin(), segment.end(), oi); 
		memset(buf, 0, sizeof(buf)); 
	}

	// free unneeded structures, close fds 
	freeaddrinfo(server_info); 
  	close(sockfd); 

  	return 0; 
}

