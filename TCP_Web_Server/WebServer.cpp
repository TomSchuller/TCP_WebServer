#include "WebServer.h"

WebServer WebServer::server;

WebServer::WebServer() {
    //sockets
    sockets = new WebSocket[MAX_SOCKETS];

    //wsaData
    if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
    {
        cout << "Web Server: Error at WSAStartup()\n";
        //TODO: exception
    }

    //listenSocket
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == listenSocket)
    {
        cout << "Web Server: Error at socket(): " << WSAGetLastError() << endl;
        WSACleanup();
        //TODO: exception
    }

    //serverService
    serverService.sin_family = AF_INET;
    serverService.sin_addr.s_addr = INADDR_ANY;
    serverService.sin_port = htons(WEB_PORT);
    if (SOCKET_ERROR == bind(listenSocket, (SOCKADDR*)&serverService, sizeof(serverService)))
    {
        cout << "Web Server: Error at bind(): " << WSAGetLastError() << endl;
        closesocket(listenSocket);
        WSACleanup();
        //TODO: exception
    }

    if (SOCKET_ERROR == listen(listenSocket, 5))
    {
        cout << "Web Server: Error at listen(): " << WSAGetLastError() << endl;
        closesocket(listenSocket);
        WSACleanup();
        //TODO: exception
    }

    addSocket(listenSocket, WebSocket::State::LISTEN);
}

WebServer::~WebServer() {
    for (int i = 0; i < MAX_SOCKETS; ++i)
    {
        cout << "Web Server: Closing Socket " << i << "." << endl;
        closesocket(sockets[i].getID());
    }
    delete[] sockets;

    cout << "Web Server: Closing Connection." << endl;
    closesocket(listenSocket);
    WSACleanup();
}

bool WebServer::addSocket(SOCKET id, WebSocket::State state) {
    for (int i = 0; i < MAX_SOCKETS; ++i)
    {
        if (sockets[i].getRecv() == WebSocket::State::EMPTY)
        {
            sockets[i].setID(id);
            sockets[i].setRecv(state);
            sockets[i].setSend(WebSocket::State::IDLE);
            return true;
        }
    }
    return false;
}
void WebServer::removeSocket(int index) {
    sockets[index].setRecv(WebSocket::State::EMPTY);
    sockets[index].setSend(WebSocket::State::EMPTY);
}
void WebServer::acceptConnection(int index) {
    SOCKET id = sockets[index].getID();
    struct sockaddr_in from;		// Address of sending partner
    int fromLen = sizeof(from);

    SOCKET msgSocket = accept(id, (struct sockaddr*)&from, &fromLen);
    if (INVALID_SOCKET == msgSocket)
    {
        cout << "Web Server: Error at accept(): " << WSAGetLastError() << endl;
        //TODO: exception
    }
    cout << "Web Server: Client " << inet_ntoa(from.sin_addr) << ":" << ntohs(from.sin_port) << " is connected." << endl;

    // Set the socket to be in non-blocking mode.
    unsigned long flag = 1;
    if (ioctlsocket(msgSocket, FIONBIO, &flag) != 0)
    {
        cout << "Web Server: Error at ioctlsocket(): " << WSAGetLastError() << endl;
        //TODO: exception

    }

    if (!addSocket(msgSocket, WebSocket::State::RECEIVE))
    {
        cout << "Too many connections, dropped!" << endl;
        closesocket(id);
        //TODO: exception
    }
}
void WebServer::receiveMessage(int index) {
    HttpRequest newReq(sockets[index]);
}
void WebServer::sendMessage(int index) {
    HttpResponse newRes(sockets[index]);
}