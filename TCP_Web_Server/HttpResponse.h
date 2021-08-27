#pragma once
#include <string>
#include <fstream>
#include <streambuf>
#include "HttpRequest.h"

using namespace std;

class HttpResponse {
private:
    string status;
    string contentLength;
    string contentType;

public:
    HttpResponse(WebSocket& socket);
    ~HttpResponse() = default;

    string createResponse(string URI);
    string doGET(string URI);
    void setStatus(WebSocket::OperationType op);
};
