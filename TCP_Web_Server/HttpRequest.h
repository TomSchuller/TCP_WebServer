#pragma once
#include "WebSocket.h"
#include <string>
#include <iostream>
#include <WinSock2.h>
#include <sstream>

using namespace std;

class HttpRequest {
private:
    string buffer;
public:
    static const int MAX_RECV_BUFF = 4096; //TODO: consider if we need more or less space.
    HttpRequest(WebSocket& socket);
    ~HttpRequest() = default;

    // Getters
    string getBuffer() { return buffer; }
};