#pragma once
#include "WebSocket.h"
#include <string>
#include <iostream>
#include <WinSock2.h>
#include <sstream>

using namespace std;

class HttpRequest {
private:
    static const int MAX_RECV_BUFF = 4096; //TODO: consider if we need more or less space.
    string buffer;

public:
    HttpRequest(WebSocket& socket);
    ~HttpRequest() = default;

    // Getters
    string getBuffer() { return buffer; }
};