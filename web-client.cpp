#include <string>
#include <thread>
#include <iostream>
#include <cstdlib> 
#include "HTTP-message.h" 

using namespace std; 

int main(int argc, char **argv)
{
	if (argc != 2){
		cerr<<"Usage: "<< argv[0] <<" <URL>\n";
		exit(1); 
	}

	// Create a HTTP Request
	string s(argv[1]); 
	HttpRequest request(s);
	cout << request.getUrl() << " " << request.getMethod() << " " << request.getHeaders()["Host"] << endl;
	return 0; 
  // do your stuff here! or not if you don't want to.
}
