#pragma once
#include <string>
#include <fstream>
#include <streambuf>
#include "HttpRequest.h"

using namespace std;

class HttpResponse {
private:
    WebSocket::OperationType operation;
    string statusCode;
    string statusMsg;
    string contentLength;
    string contentType;

public:
    HttpResponse(WebSocket& socket);
    ~HttpResponse() = default;

    string createResponse(WebSocket& socket);
    string doGET(WebSocket& socket);
    string doPOST(WebSocket& socket);
    string doOPTIONS(WebSocket& socket);
    string doPUT(WebSocket& socket);
    string doDELETE(WebSocket& socket);
    string doTRACE(WebSocket& socket);
    string doHEAD(WebSocket& socket);
    string doERROR();
};