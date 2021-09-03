/////////////////////////////////
// This Class defines an HTTP Request.
/////////////////////////////////
#pragma once
#include "WebSocket.h"
#include <string>
#include <iostream>
#include <WinSock2.h>
#include <sstream>
#include "WebServerException.h"

using namespace std;

class HttpRequest {
private:
    string buffer;
public:
    static const int MAX_RECV_BUFF = 4096; 
    HttpRequest(WebSocket& socket);
    ~HttpRequest() = default;

    // Getters
    string getBuffer() { return buffer; }
};