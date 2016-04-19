#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string> 
#include <iostream>
#include <sstream>
#include <cstdlib> 
#include <netdb.h>
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
	ByteBlob message = request.encode();  
	std::string out(message.begin(), message.end());
	std::cout << out;
	std::cout << request.getPortNum() << std::endl; 

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
	char buf[8192];
	ByteBlob copy_of_request = request.encode();
	int size = copy_of_request.size();  
	ret = write(sockfd, copy_of_request.data(), size); 
	if (ret < 0)
		perror("socket write failed");
	while (ret > 0) {
		memset(buf, 0, 8192); 
		ret = read(sockfd, buf, 8192); 
		if (ret < 0)
			error("socket read failed"); 
		std::cout << buf;
	}
	freeaddrinfo(servinfo); // all done with this structure
  	close(sockfd); 
  	return 0; 
}

