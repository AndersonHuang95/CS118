#include "HTTP-message.h"

#include <iostream>
#include <sstream> 

using namespace std;

///////////////////////////////////////
/*                                   */
/* HttpMessage functions             */
/*                                   */
///////////////////////////////////////

// HTTP 1.0 by default 
HttpMessage :: HttpMessage(){
	m_version = "1.0"; 
}

////////////////////
// Getters 
/////////////////// 
HttpVersion HttpMessage :: getVersion() {
  return m_version;
}

string HttpMessage :: getHeader(string key) {
  return m_headers[key];
}

std::map<std::string, std::string> HttpMessage :: getHeaders() {
	return m_headers;
}

ByteBlob HttpMessage :: getPayload() {
  return m_payload;
}

////////////////////
// Setters
/////////////////// 
void HttpMessage :: setVersion(string version){
	m_version = version; 
}

void HttpMessage :: setHeader(string key, string value) {
  m_headers[key] = value;
}

void HttpMessage :: setPayLoad(ByteBlob payload) {
  m_payload = payload;
}


//////////////////////////////////////
/*                                  */
/* HttpRequest Functions            */
/*                                  */
//////////////////////////////////////

////////////////////
// Constructors 
/////////////////// 
HttpRequest :: HttpRequest(string url) {
	setMethod("GET");
	int index = 0;
	while (url[index] != '/') // Skips http://
		index++;
	index += 2;
	string host, port_num = "80"; 
	while (url[index] != '/'){
		if (url[index] == ':')
			break; 
		host += url[index++];
	}
	if(url[index] == ':'){
		// reset port_num if one is specified in URL 
		port_num = ""; 
		index++; 
		while(url[index] != '/')
			port_num += url[index++]; 
	}
	setHeader("Host", host);
	setPortNum(port_num); 
	setUrl(url.substr(index));

	setHeader("Accept", "*/*");
	setHeader("Connection", "Close"); // non-persistent
}

HttpRequest :: HttpRequest(ByteBlob wire) {
	int index = 0;
	
	// method
	while (wire[index] != ' ')
		m_method += wire[index++];
	index++;

	// url
	while (wire[index] != ' ')
		m_url += wire[index++];
	index++;

	// skip HTTP/
	while (wire[index] != '/')
		index++;
	
	// version
	string version; 
	while (wire[index] != '\r')
		version += wire[index++];
	setVersion(version); 

	index += 2;

	// headers
	while (wire[index] != '\r') {
		string key;
		string value;
		while (wire[index] != ':')
			key += wire[index++];
		index++;
		while (wire[index] != '\r')
			value += wire[index++];
		index += 2;
		setHeader(key, value);
	}
}

///////////////////////////
/// Getters 
///////////////////////////
HttpMethod HttpRequest :: getMethod() {
  return m_method;
}

string HttpRequest :: getUrl() {
  return m_url;
}

string HttpRequest :: getPortNum(){
	return m_port; 
}

///////////////////////////
/// Setters 
///////////////////////////
void HttpRequest :: setMethod(HttpMethod method) {
  m_method = method;
}


void HttpRequest :: setUrl(string url) {
  m_url = url;
}

void HttpRequest :: setPortNum(string port_num){
	m_port = port_num;
}

////////////////////////////
/// Create a HttpRequest in byte format
///////////////////////////
ByteBlob HttpRequest :: encode() {
	string firstLine = getMethod() + " " + getUrl() + " HTTP/" + getVersion() + "\r\n";
	map<string, string> headers = getHeaders();
	string headerLines;
	for(map<string, string>::iterator it = headers.begin(); it != headers.end(); ++it) {
		headerLines += it->first + ": " + it->second + "\r\n";
	}
	string HTTP = firstLine + headerLines + "\r\n";
	return ByteBlob(HTTP.begin(), HTTP.end());
}

/////////////////////////////////////
/*                                 */
/* HttpResponse Functions          */
/*                                 */
/////////////////////////////////////

/////////////////////////////
/// Getters
/////////////////////////////
HttpStatus HttpResponse :: getStatus() {
  return m_status;
}

string HttpResponse :: getDescription() {
  return m_statusDescription;
}

/////////////////////////////
/// Setters
/////////////////////////////
void HttpResponse :: setStatus(HttpStatus status) {
  m_status = status;
}

void HttpResponse :: setDescription(string description) {
  m_statusDescription = description;
}

////////////////////////////
/// Create a HttpResponse in byte format 
///////////////////////////
ByteBlob HttpResponse :: encode() {
	string firstLine = "HTTP/" + getVersion() + " " + getStatus() + " " + getDescription() + "\r\n"; 
	//cout << "setup" << endl;
	map<string, string> headers = getHeaders();
	//cout << "headers" << endl;
	string headerLines;
	for(map<string, string>::iterator it = headers.begin(); it != headers.end(); ++it) {
		headerLines += it->first + ": " + it->second + "\r\n";
		//cout << "First: " << it->first << endl;
		//cout << "Second: " << it->second << endl;
	}
	// cout << "sizeof firstLine: " << firstLine.size() << endl;
    //cout << "sizeof headerLines: " << headerLines.size() << endl;
    // 	cout << "sizeof payload: " << getPayload().size() << endl;
	ByteBlob p = getPayload();
	string pay;
	for (unsigned int i = 0; i < p.size(); ++i)
		pay += p[i];
	//cout << "sizeof pay: " << pay.size() << endl;
	string HTTP = firstLine + headerLines + "\r\n" + pay;
	
	return ByteBlob(HTTP.begin(), HTTP.end());
}
