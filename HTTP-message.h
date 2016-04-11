#include <string>
#include <iostream>
#include <map>

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
	void setPayLoad(vector<uint_8> payload);
	vector<uint_8> getPayload();
private:
	string m_version;
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
	std::string m_method;
	std::string m_url;
};

class HttpResponse : public HttpMessage{
public:
	virtual void decodeFirstLine(vector<uint_8> line);
	std::string getStatus();
	void setStatus(std::string status);
	std::string getDescription();
	void setDescription(std::string description);
private:
	std::string m_status;
	std::string m_statusDescription;
};
