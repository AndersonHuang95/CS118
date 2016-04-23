#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string> 
#include <iostream>	// consoel I/O
#include <fstream> 	// file I/O
#include <sstream>
#include <cstdlib> 
#include <netdb.h>
#include <iterator> 
#include "HTTP-message.h" 

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int
main(int argc, char **argv)
{
	// Parse command line args
	if (argc != 2){
		std::cerr<<"Usage: "<< argv[0] <<" <URL>\n";
		exit(1); 
	}

	// Create a HTTP Request
	std::string s(argv[1]); 
	HttpRequest request(s);
	/*
	ByteBlob message = request.encode();  
	std::string out(message.begin(), message.end());
	std::cout << out;
	std::cout << request.getPortNum() << std::endl; 
	*/

	// code for a client connecting to a server
	// namely a stream socket to www.example.com on port 80 (http)
	// either IPv4 or IPv6

	int sockfd; // client socket  
	struct addrinfo hints, *servinfo, *p;	
	int ret;	// check success/failure of socket API calls 

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
	hints.ai_socktype = SOCK_STREAM;

	std::map<std::string, std::string> headers = request.getHeaders(); 
	if ((ret = getaddrinfo(headers["Host"].c_str(), request.getPortNum().c_str(), &hints, &servinfo)) != 0) {
	    std::cerr << "getaddrinfo: %s\n", gai_strerror(ret);
	    exit(1);
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		
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

	    break; // if we get here, we must have connected successfully
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
	if (ret < 0)
		perror("socket write failed");

	// Get the filename 
	std::string path = request.getUrl(); 
	std::stringstream ss(path); 
	std::string token, filename; 

	while(std::getline(ss, token, '/')){
		filename = token; 
	}
	// TODO: 
	// Parse HTTPResponse
	// Handle errors accordingly 
	// Open a output file stream if successful and stuff data there 
	char buf[8192];
	memset(buf, 0, sizeof(buf)); 
	while ( (ret = read(sockfd, buf, sizeof(buf))) != 0) {
		// std::cout << ret << std::endl;
		if (ret < 0)
			error("socket read failed"); 
		response.insert(response.end(), buf, buf + strlen(buf));
		memset(buf, 0, sizeof(buf)); 
	}

	// Parse reponse for HttpResponse Code
	int index = 0; 
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
		std::cout << "200 OK\n";
	else if (status_code == "400")
		std::cout << "400 Bad Request\n";
	else if (status_code == "404")
		std::cout << "404 Not Found\n";
	else if (status_code == "403")
		std::cout << "403 Forbidden\n";
	else if (status_code == "501")
		std::cout << "501 Not Implemented\n"; 
	else if (status_code == "505")
		std::cout << "505 HTTP version not supported\n";
	else
		std::cout << "Unknown error code encountered\n";

	while(response[index] != '\r' || response[index + 1] != '\n' || response[index + 2] != '\r' || response[index + 3] != '\n')
		index++;
	index = index + 4; 
	// Erase first (index) bytes
	/*
	for(int j = 0; j < response.size(); j++)
		std::cout << response[j]; 
	*/
	
	// response.erase(response.begin(), response.begin() + index); 
	// Store remaining reponse into filename
	std::ofstream outfile(filename);
	std::ostream_iterator<uint8_t> oi(outfile, ""); 
	std::copy(response.begin() + index, response.end(), oi); 
	// free unneeded structures, close fds 
	freeaddrinfo(servinfo); // all done with this structure
  	close(sockfd); 

  	return 0; 
}

