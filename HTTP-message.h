#include <string>
#include <iostream>
#include <map>

enum HttpVersion {1.0, 1.1, 2.0};

vector<uint8_t> encode(string message) {
	vector<uint8_t> enc;
	for (int i = 0; i < message.size(); i++) {
		enc.push_back((uint8_t)message[i]);
	}
	return enc;
}

// HttpMessage vs. string
string decode(vector<uint8_t> message) {
	string dec;
	for (int i = 0; i < message.size(); i++) {
		dec += (char)message[i];
	}
	return dec;
}

class HttpMessage {
public:
	virtual void decodeFirstLine(vector<uint_8> line) = 0;
	HttpVersion getVersion();
	void setHeader(string key, string value);
	string getHeader(string key);
	void decodeHeaderLine(vector<uint_8> line);
	void setPayLoad(vector<uint_8> blob);
	vector<uint_8> getPayload();
private:
	double m_version;
	map<string, string> m_headers;
	vector<uint_8> m_payload;
};

class HttpRequest : public HttpMessage{
public:
	virtual void decodeFirstLine(vector<uint_8> line);
	HttpMethod getMethod();
	void setMethod(HttpMethod method);
	string getUrl();
	void setUrl(string url);
private:
	HttpMethod m_method;
	string m_url;
};

class HttpResponse : public HttpMessage{
public:
	virtual void decodeFirstLine(vector<uint_8> line);
	HttpStatus getStatus();
	void setStatus(HttpStatus status);
	string getDescription();
	void setDescription(string description);
private:
	HttpStatus m_status;
	string m_statusDescription;
};
