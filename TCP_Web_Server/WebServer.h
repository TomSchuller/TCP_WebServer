#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "WebSocket.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

using namespace std;

class WebServer {
private:
    //Const Variables
    static const int WEB_PORT = 80;

    //Singleton
    static WebServer server;

    //Constructors
    WebServer();
    WebServer(const WebServer&) = delete;

    //Variables
    WebSocket* sockets = nullptr;
    WSAData wsaData;
    SOCKET listenSocket;
    sockaddr_in serverService;
    fd_set waitRecv;
    fd_set waitSend;
    

public:
    static const int MAX_SOCKETS = 60;
    //dtor
    ~WebServer();

    //Getters
    static WebServer& getInstance() { return server; }

    //Methods
    WebSocket& getSocket(int index) { return sockets[index]; }
    bool addSocket(SOCKET& id, WebSocket::State state);
    void removeSocket(int index);
    void acceptConnection(int index);
    void receiveMessage(int index);
    void sendMessage(int index);
    int getWaitingSockets();
    void HandleRecv(int& nfd);
    void HandleSend(int& nfd);
};