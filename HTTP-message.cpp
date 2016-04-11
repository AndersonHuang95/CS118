#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include "HTTP-message.h"


/*
	HttpMessage Functions
*/

std::string HttpMessage :: getVersion() {
	return m_version;
}

void HttpMessage :: setHeader(std::string key, std::string value) {
	m_headers[key] = value;
}

std::string HttpMessage :: getHeader(std::string key) {
	return m_headers[key];
}

// Might miss if more than just 1 space???
void HttpMessage :: decodeHeaderLine(vector<uint_8> line) {
	std::stringstream ss(decode(line));
	std::string key, value;
	ss >> key >> value;
	key.pop_back(); // Get rid of ':'
	m_headers[key] = value;
}

void HttpMessage :: setPayLoad(vector<uint_8> payload) {
	m_payload = payload;
}

vector<uint_8> HttpMessage :: getPayload() {
	return m_payload;
}


/*
	HttpRequest Functions
*/

virtual void HttpRequest :: decodeFirstLine(vector<uint_8> line) {
	std::stringstream ss(decode(line));
	ss >> m_method >> m_url >> m_version;
}

std::string HttpRequest :: getMethod() {
	return m_method;
}

void HttpRequest :: setMethod(std::string method) {
	m_method = method;
}

std::string HttpRequest :: getUrl() {
	return m_url;
}

void HttpRequest :: setUrl(std::string url) {
	m_url = url;
}

/*
	HttpResponse Functions
*/

virtual void HttpResponse :: decodeFirstLine(vector<uint_8> line) {	
	std::stringstream ss(decode(line));
	ss >> m_version >> m_status >> m_statusDescription;
}

std::string HttpResponse :: getStatus() {
	return m_status;
}

void HttpResponse :: setStatus(std::string status) {
	m_status = status;
}

std::string HttpResponse :: getDescription() {
	return m_description;
}

void HttpResponse :: setDescription(std::string description) {
	m_statusDescription = description;
}
