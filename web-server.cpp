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

#include <string>
#include <iostream>
#include <sstream>
#include <thread>

using namespace std;

void threadFunc(int clientSockfd) {
  // read/write data from/into the connection
  ByteBlob buf(8192);

	int nbytes = recv(clientSockfd, &buf[0], buf.size(), 0);
	if(nbytes > buf.size()) {
	  buf.resize(nbytes);
	  nbytes = recv(clientSockfd, &buf[0], buf.size(), 0);
	}
	if(nbytes == -1) {
	  perror("recv");
	}

	HttpRequest h(buf);
	HttpResponse r();
	
	r.setStatus("200");
	r.setDescription("OK");
	r.setVersion("1.0");
	r.setHeader("Date", "Sat, 23 Apr 2016 19:51:20 GMT");
	r.setHeader("Expires", "-1");
	r.setHeader("Cache-Control", "private, max-age=0");
	r.setHeader("Content-Type", "text/html; charset=ISO-8859-1");
	r.setHeader("P3P", "CP='This is not a P3P policy! See https://www.google.com/support/accounts/answer/151657?hl=en for more info.'");
	r.setHeader("Server", "gws");
	r.setHeader("X-XSS-Protection", "1; mode=block");
	r.setHeader("X-Frame-Options", "SAMEORIGIN");
	r.setHeader("Set-Cookie", "NID=78=OXe088LKrMmjLmlP4dzimxC83BjfzpSLBAclvzcwtS8J_w1OQmYa4HggH9IhOGfrTj2yVcU_WGem16hDgX3NPSjW1hzM3CTMkpSUfi2Eze-4AaGXRWO9kHzM2YJP8NgN5fxOw2jAA0s0Y3A; expires=Sun, 23-Oct-2016 19:51:20 GMT; path=/; domain=.google.com; HttpOnly");
	r.setHeader("Accept-Ranges", "none");
	r.setHeader("Vary", "Accept-Encoding");

	int fd = open(h.getURL(), O_RDONLY);
	if (fr ==  NULL) {
		fprintf(stderr, "Error opening file --> %s", strerror(errno));
		exit(EXIT_FAILURE);
	}


	/* Get file stats */
	struct stat file_stat;
	if (fstat(fd, &file_stat) < 0) {
		fprintf(stderr, "Error fstat --> %s", strerror(errno));
		exit(EXIT_FAILURE);
	}
	r.setHeader("Content-Length", file_stat.st_size);
	offset = 0;
	remain_data = file_stat.st_size;
	/* Sending file data */
	while (((sent_bytes = sendfile(peer_socket, fd, &offset, BUFSIZ)) > 0) && (remain_data > 0)) {
		fprintf(stdout, "1. Server sent %d bytes from file's data, offset is now : %d and remaining data = %d\n", sent_bytes, offset, remain_data);
		remain_data -= sent_bytes;
		fprintf(stdout, "2. Server sent %d bytes from file's data, offset is now : %d and remaining data = %d\n", sent_bytes, offset, remain_data);
	}

	close(clientSockfd);
}


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

  // prepare hints
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET; // IPv4
  hints.ai_socktype = SOCK_STREAM; // TCP

  // get address
  int status = 0;
  if ((status = getaddrinfo(hostName, port, &hints, &res)) != 0) {
    cerr << "getaddrinfo: " << gai_strerror(status) << endl;
    return 2;
  }

  struct addrinfo* p = res;
  // convert address to IPv4 address
  struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
  
  // convert the IP to a string and print it:
  char ipstr[INET_ADDRSTRLEN] = {'\0'};
  inet_ntop(p->ai_family, &(ipv4->sin_addr), ipstr, sizeof(ipstr));
  
  freeaddrinfo(res); // free the linked list

  // create a socket using TCP IP
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // allow others to reuse the address
  int yes = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    perror("setsockopt");
    return 1;
  }

  // bind address to socket
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(atoi(port));     // short, network byte order
  addr.sin_addr.s_addr = inet_addr(hostName);
  memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

  if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("bind");
    return 2;
  }

  // set socket to listen status
  if (listen(sockfd, 1) == -1) {
    perror("listen");
    return 3;
  }


  // accept a new connection
  int clientSockfd;
  while(true) {
    struct sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    clientSockfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientAddrSize);
    
    if (clientSockfd == -1) {
      perror("accept");
    return 4;
    }
    thread(threadFunc, clientSockfd);
  }

  return 0;
}
