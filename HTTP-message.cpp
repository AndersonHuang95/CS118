#include "HTTP-message.h"

#include <iostream>
#include <sstream> // Maybe don't need if take out decode lines??????

using namespace std;

///////////////////////////////////////
/*                                   */
/* HttpMessage functions             */
/*                                   */
///////////////////////////////////////

HttpVersion HttpMessage :: getVersion() {
  return m_version;
}

void HttpMessage :: setHeader(string key, string value) {
  m_headers[key] = value;
}

string HttpMessage :: getHeader(string key) {
  return m_headers[key];
}

// Might miss if more than just 1 space???
// Do we need this????????
void HttpMessage :: decodeHeaderLine(ByteBlob line) {
  stringstream ss(decode(line));
  string key, value;
  ss >> key >> value;
  key.pop_back(); // Get rid of ':'
  m_headers[key] = value;
}

void HttpMessage :: setPayLoad(ByteBlob payload) {
  m_payload = payload;
}

ByteBlob HttpMessage :: getPayload() {
  return m_payload;
}

std::map<std::string, std::string> HttpMessage :: getHeaders() {
	return m_headers;
}


//////////////////////////////////////
/*                                  */
/* HttpRequest Functions            */
/*                                  */
//////////////////////////////////////

HttpRequest :: HttpRequest(string url) {
	setMethod("GET");
	int index = 0;
	while (url[index] != '/') // Skips http://
		index++;
	index += 2;
	string host;
	while (url[index] != '/')
		host += url[index++];
	setHeader("Host", host);
	setUrl(url.substr(index));
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
	while (wire[index] != '\r')
		m_version += wire[index++];

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

// Do we need this????????
void HttpRequest :: decodeFirstLine(ByteBlob line) {
  stringstream ss(decode(line));
  ss >> m_method >> m_url;
}


HttpMethod HttpRequest :: getMethod() {
  return m_method;
}

void HttpRequest :: setMethod(HttpMethod method) {
  m_method = method;
}

string HttpRequest :: getUrl() {
  return m_url;
}

void HttpRequest :: setUrl(string url) {
  m_url = url;
}

ByteBlob HttpRequest :: encode() {
	string firstLine = getMethod() + " " + getUrl() + " HTTP/" + getVersion() + "/r/n";
	map<string, string> headers = getHeaders();
	string headerLines;
	for(const auto it = headers.begin(); it != headers.end(); ++it) {
		headerLines += it->first + ": " + it->second + "/r/n";
	}
	string HTTP = firstLine + headerLines + "/r/n";
	return ByteBlob(HTTP.begin(), HTTP.end());
}

/////////////////////////////////////
/*                                 */
/* HttpResponse Functions          */
/*                                 */
/////////////////////////////////////

// Do we need this????????
void HttpResponse :: decodeFirstLine(ByteBlob line) {	
  stringstream ss(decode(line));
  HttpVersion v;
  ss >> v >> m_status >> m_statusDescription;
  setVersion(v);
}

HttpStatus HttpResponse :: getStatus() {
  return m_status;
}

void HttpResponse :: setStatus(HttpStatus status) {
  m_status = status;
}

string HttpResponse :: getDescription() {
  return m_statusDescription;
}

void HttpResponse :: setDescription(string description) {
  m_statusDescription = description;
}

ByteBlob HttpResponse :: encode() {
	string firstLine = "HTTP/" + getVersion() + " " + getStatus() + " " + getDescription() + "/r/n"; 
	map<string, string> headers = getHeaders();
	string headerLines;
	for(const auto it = headers.begin(); it != headers.end(); ++it) {
		headerLines += it->first + ": " + it->second + "/r/n";
	}
	string HTTP = firstLine + headerLines + "/r/n" + getPayload();
	return ByteBlob(HTTP.begin(), HTTP.end());
}

