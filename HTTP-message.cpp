#include <string>
#include <iostream>
#include "HTTP-message.h"


/*
	HttpMessage Functions
*/

HttpVersion HttpMessage :: getVersion() {
	return m_version;
}

void HttpMessage :: setHeader(string key, string value) {
	m_headers[key] = value;
}

string HttpMessage :: getHeader(string key) {
	return m_headers[key];
}

// TODO
void HttpMessage :: decodeHeaderLine(vector<uint_8> line);

void HttpMessage :: setPayLoad(vector<uint_8> blob) {
	m_payload = blob;
}

vector<uint_8> HttpMessage :: getPayload() {
	return m_payload;
}


/*
	HttpRequest Functions
*/

// TODO
virtual void HttpRequest :: decodeFirstLine(vector<uint_8> line);

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

/*
	HttpResponse Functions
*/

// TODO
virtual void HttpResponse :: decodeFirstLine(vector<uint_8> line) {	

}

HttpStatus HttpResponse :: getStatus() {
	return m_status;
}

void HttpResponse :: setStatus(HttpStatus status) {
	m_status = status;
}

string HttpResponse :: getDescription() {
	return m_description;
}

void HttpResponse :: setDescription(string description) {
	m_description = description;
}
