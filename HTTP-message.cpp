#include "HTTP-message.h"

#include <iostream>
#include <sstream>

using namespace std;

///////////////////////////////////////
/*                                   */
/* HttpMessage functions             */
/*                                   */
///////////////////////////////////////

HttpMessage :: HttpMessage() 
	: m_version("1.0")
{}

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

std::map<std::string, std::string> getHeaders() {
	return m_headers;
}


//////////////////////////////////////
/*                                  */
/* HttpRequest Functions            */
/*                                  */
//////////////////////////////////////

void HttpRequest :: decodeFirstLine(ByteBlob line) {
  stringstream ss(decode(line));
  string v;
  ss >> m_method >> m_url >> v;
  setVersion(v);
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

