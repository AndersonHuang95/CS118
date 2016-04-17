#include "HTTP-message.h"

#include <iostream>
#include <sstream>

using namespace std;

ByteBlob encode(string message) {
  ByteBlob enc;
  for (int i = 0; i < message.size(); i++) {
    enc.push_back((uint8_t)message[i]);
  }
  return enc;
}

string decode(ByteBlob message) {
  string dec;
  for (int i = 0; i < message.size(); i++) {
    dec += (char)message[i];
  }
  return dec;
}

///////////////////////////////////////
/*                                   */
/* HttpMessage functions             */
/*                                   */
///////////////////////////////////////

HttpVersion HttpMessage :: getVersion() {
  return m_version;
}

void HttpMessage::setVersion(HttpVersion version) {
  m_version = version;
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
