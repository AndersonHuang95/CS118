#ifndef HTTP_H
#define HTTP_H

#include <string>
#include <vector>
#include <map>

typedef std::vector<uint8_t> ByteBlob;
typedef std::string HttpVersion;
typedef std::string HttpMethod;
typedef std::string HttpStatus;

class HttpMessage {
public:
  HttpMessage();
  virtual void decodeFirstLine(ByteBlob line) = 0;
  virtual ByteBlob encode() = 0;
  HttpVersion getVersion();
  void setHeader(std::string key, std::string value);
  std::string getHeader(std::string key);
  void decodeHeaderLine(ByteBlob line);
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
  virtual void decodeFirstLine(ByteBlob line);
  HttpMethod getMethod();
  void setMethod(HttpMethod method);
  std::string getUrl();
  void setUrl(std::string url);
  ByteBlob encode();
private:
  HttpMethod  m_method;
  std::string m_url;
};

class HttpResponse : public HttpMessage {
public:
  virtual void decodeFirstLine(ByteBlob line);
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
