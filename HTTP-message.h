#ifndef HTTP_H
#define HTTP_H

#include <string>
#include <vector>
#include <map>
#include <stdint.h> // uint8_t

typedef std::vector<uint8_t> ByteBlob;
typedef std::string HttpVersion;
typedef std::string HttpMethod;
typedef std::string HttpStatus;

class HttpMessage {
public:
  HttpMessage(); 
  virtual ByteBlob encode() = 0; // not defined 
  HttpVersion getVersion();
  void setVersion(std::string version); 
  void setHeader(std::string key, std::string value);
  std::string getHeader(std::string key);
  void setPayLoad(ByteBlob payload);
  ByteBlob getPayload();
  std::map<std::string, std::string> getHeaders();
private:
  HttpVersion  m_version;
  std::map<std::string, std::string> m_headers;
  ByteBlob m_payload;
};

class HttpRequest : public HttpMessage {
public:
  HttpRequest(std::string url);
  HttpRequest(ByteBlob wire);
  HttpMethod getMethod();
  void setMethod(HttpMethod method);
  std::string getUrl();
  void setUrl(std::string url);
  std::string getPortNum(); 
  void setPortNum(std::string port_num); 
  ByteBlob encode();
private:
  HttpMethod  m_method;
  std::string m_url;
  std::string m_port; // Added port number 
};

class HttpResponse : public HttpMessage {
public:
  HttpStatus getStatus();
  void setStatus(std::string status);
  std::string getDescription();
  void setDescription(std::string description);
  ByteBlob encode();
private:
  HttpStatus m_status;
  std::string m_statusDescription;
};
#endif
