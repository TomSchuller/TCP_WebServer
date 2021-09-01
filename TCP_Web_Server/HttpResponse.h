#pragma once
#include <string>
#include <fstream>
#include <streambuf>
#include "HttpRequest.h"


using namespace std;

class HttpResponse {
private:
    WebSocket::OperationType operation;
    string lang;
    string uri;

    string statusCode;
    string statusMsg;
    string contentLength;
    string contentType;

    //Parsers
    static WebSocket::OperationType parseOperation(const string& buffer);
    static string parseLang(const string& buffer);
    static string parseURI(const string& buffer);
    static string parseBody(const string& buffer);

public:
    HttpResponse(WebSocket& socket);
    ~HttpResponse() = default;
    
    // Getters
    const WebSocket::OperationType& getOp() const { return operation; }
    const std::string& getAsset() const { return uri; }
    const std::string& getLang() const { return lang; }

    string createResponse(WebSocket& socket);
    string doGET(WebSocket& socket);
    string doPOST(WebSocket& socket);
    string doOPTIONS(WebSocket& socket);
    string doPUT(WebSocket& socket);
    string doDELETE(WebSocket& socket);
    string doTRACE(WebSocket& socket);
    string doHEAD(WebSocket& socket);
};