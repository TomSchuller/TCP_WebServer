#pragma once
#include "WebSocket.h"
#include <string>
#include <iostream>
#include <WinSock2.h>
#include <sstream>

using namespace std;

class HttpRequest {
private:
    static const int MAX_RECV_BUFF = 1024;
    WebSocket::OperationType operation;
    string lang;
    string uri;

    //Methods
    static WebSocket::OperationType parseOperation(string& buffer);
    static string parseLang(string& buffer);
    static string parseURI(string& buffer);
public:
    HttpRequest(WebSocket& socket);
    ~HttpRequest() = default;

    // Getters
    WebSocket::OperationType getOperation() { return operation; }
    string getLang() { return lang; }
    string getURI() { return uri; }
};