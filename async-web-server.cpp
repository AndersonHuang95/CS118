#include "HTTP-message.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/select.h>

#include <string>
#include <iostream>
#include <sstream>
#include <thread>

#define TIMEOUT_SEC 15

using namespace std;

int main(int argc, char **argv)
{
	string hostName = "localhost";
	string port = "4000";
	string fileDir = ".";
	if(argc >= 2) {
		hostName = argv[1];
	}
	if(argc >= 3) {
		port = argv[2];
	}
	if(argc >= 4) {
		fileDir = argv[3];
	}

	struct addrinfo hints;
	struct addrinfo* res;

	fd_set readFds;
	fd_set errFds;
	fd_set watchFds;
	FD_ZERO(&readFds);
	FD_ZERO(&errFds);
	FD_ZERO(&watchFds);

  	// create a socket using TCP IP
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	int maxSockfd = sockfd;

  	// put the socket in the socket set
	FD_SET(sockfd, &watchFds);

  	// prepare hints
	memset(&hints, 0, sizeof(hints));
  	hints.ai_family = AF_INET; // IPv4
  	hints.ai_socktype = SOCK_STREAM; // TCP

  	// get address
  	int status = 0;
  	if ((status = getaddrinfo(hostName.c_str(), port.c_str(), &hints, &res)) != 0) {
  		cerr << "getaddrinfo: " << gai_strerror(status) << endl;
  		return 2;
  	}
		//cout << "Resolve" << endl;
	struct addrinfo* p = res;
	  // convert address to IPv4 address	
  	struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
  
  	// convert the IP to a string and print it:
  	char ipstr[INET_ADDRSTRLEN] = {'\0'};
  	inet_ntop(p->ai_family, &(ipv4->sin_addr), ipstr, sizeof(ipstr));
  
  	freeaddrinfo(res); // free the linked list

  	// allow others to reuse the address
  	int yes = 1;
  	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
  		perror("setsockopt");
  		return 1;
  	}
	//cout << "Bind" << endl;  // bind address to socket
  	struct sockaddr_in addr;
  	addr.sin_family = AF_INET;
  	addr.sin_port = htons(stoi(port));     // short, network byte order
  	//cout << ipstr << endl;
  	addr.sin_addr.s_addr = inet_addr(ipstr);
  	memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

  	if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
  		perror("bind");
  		return 2;
  	}
  
  	if (listen(sockfd, 1) == -1) {
  		perror("listen");
  		return 3;
  	}

  	// initialize timer (2s)
  	struct timeval tv;
  	while (true) {
  	  // set up watcher
  		int nReadyFds = 0;
  		readFds = watchFds;
  		errFds = watchFds;
  		tv.tv_sec = 3;
  		tv.tv_usec = 0;
  		if ((nReadyFds = select(maxSockfd + 1, &readFds, NULL, &errFds, &tv)) == -1) {
  			perror("select");
  			return 4;
  		}
  		if (nReadyFds == 0) {
  			std::cout << "no data is received for 3 seconds!" << std::endl;
  		}
  		else {
  			for(int fd = 0; fd <= maxSockfd; fd++) {
        	// get one socket for reading
  				if (FD_ISSET(fd, &readFds)) {
  					if (fd == sockfd) { // this is the listen socket
  						struct sockaddr_in clientAddr;
  						socklen_t clientAddrSize = sizeof(clientAddr);
  						int clientSockfd = accept(fd, (struct sockaddr*)&clientAddr, &clientAddrSize);

  						if (clientSockfd == -1) {
  							perror("accept");
  							return 5;
  						}

  						char ipstr[INET_ADDRSTRLEN] = {'\0'};
  						inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, ipstr, sizeof(ipstr));
            			//std::cout << "Accept a connection from: " << ipstr << ":" <<
            			//ntohs(clientAddr.sin_port) << " with Socket ID: " << clientSockfd << std::endl;

            			// update maxSockfd
  						if (maxSockfd < clientSockfd)
  							maxSockfd = clientSockfd;

            			// add the socket into the socket set
  						FD_SET(clientSockfd, &watchFds);
  					}
  					else {
  						int clientSockfd = fd;
  						struct timeval timeout;      
    					timeout.tv_sec = TIMEOUT_SEC;
    					timeout.tv_usec = 0;

   						if (setsockopt (clientSockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
        					close(clientSockfd);
        					FD_CLR(clientSockfd, &watchFds);

   						}

    					if (setsockopt (clientSockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
        					close(clientSockfd);
        					FD_CLR(clientSockfd, &watchFds);

    					}
          				// this is the normal socket
          				// read/write data from/into the connection
  						ByteBlob buf(8192);
  						int nbytes = recv(clientSockfd, &buf[0], buf.size(), 0);
  						if(nbytes > (int)buf.size()) {
  							buf.resize(nbytes);
  							nbytes = recv(clientSockfd, &buf[0], buf.size(), 0);
  						}
  						if(nbytes == -1) {
  							perror("recv");
  						}
						//cout << "recv done" << endl;

  						HttpRequest h(buf);
  						HttpResponse r;

  						r.setVersion("1.0");

  						if(h.getMethod() != "GET" || (h.getVersion() != "/1.0" && h.getVersion() != "/1.1")) {
							//cout << "Error 400" << endl;
							//cout << h.getMethod() << endl;
							//cout << h.getVersion() << endl;
  							r.setStatus("400");
  							r.setDescription("Bad request");
  						}
  						else {
  							string str(buf.begin(), buf.end());
							//cout << "Message: " << str << endl;
							//cout << "Filename is: " <<  h.getUrl().substr(1).c_str() << endl;
  							int fd = open(h.getUrl().substr(1).c_str(), O_RDONLY);
  							if (fd ==  -1) {
								//cout << "Error 404" << endl;
  								r.setStatus("404");
  								r.setDescription("Not found");
  							}	
  							else {
  								r.setStatus("200");
  								r.setDescription("OK");
  								uint8_t fileBuf[8192];
  								ByteBlob payload;
  								int ret;
  								bool isFirst = true;
  								memset(fileBuf, 0, sizeof(fileBuf)); 
  								while ( (ret = read(fd, fileBuf, sizeof(fileBuf))) != 0) {
  									if (ret < 0)
  										perror("file read failed"); 

  									payload.insert(payload.end(), fileBuf, fileBuf + ret);
  									memset(fileBuf, 0, sizeof(fileBuf));
  									if(payload.size() >= 8193) {
  										if(isFirst) {
  											r.setHeader("Content-Length", to_string(payload.size()));
  											r.setPayLoad(payload);
  											ByteBlob sendBuf = r.encode(); 
  											ret = write(clientSockfd, sendBuf.data(), sendBuf.size());
  											isFirst = false;
  											payload.clear(); 
  										}
  										else {
  											ret = write(clientSockfd, payload.data(), payload.size());
  											payload.clear();
  										}
  										if (ret < 0)
  											perror("socket write failed");
  									}			 
  								}
  								if(!payload.empty()) {
  									if(isFirst) {
  										r.setHeader("Content-Length", to_string(payload.size()));
  										r.setPayLoad(payload);
  										ByteBlob sendBuf = r.encode(); 
  										ret = write(clientSockfd, sendBuf.data(), sendBuf.size());
  										isFirst = false;
  										payload.clear(); 
  									}
  									else {
  										ret = write(clientSockfd, payload.data(), payload.size());
  										payload.clear();
  									}
  									if (ret < 0)
  										perror("socket write failed");
  								}
  							}	
  						}
  						close(clientSockfd);
  						FD_CLR(clientSockfd, &watchFds);

  					}
  				}
  			}
  		}
  	}
  	return 0;
}
