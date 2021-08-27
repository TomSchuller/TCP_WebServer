#include "WebServer.h"

WebServer WebServer::server;

WebServer::WebServer() {
    //sockets
    sockets = new WebSocket[MAX_SOCKETS];

    //wsaData
    if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
    {
        cout << "Web Server: Error at WSAStartup()." << endl;
        exit(1);
    }

    //listenSocket
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == listenSocket)
    {
        cout << "Web Server: Error at socket(): " << WSAGetLastError() << endl;
        WSACleanup();
        exit(1);
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
        exit(1);
    }

    if (SOCKET_ERROR == listen(listenSocket, 5))
    {
        cout << "Web Server: Error at listen(): " << WSAGetLastError() << endl;
        closesocket(listenSocket);
        WSACleanup();
        exit(1);
    }

    try {
        addSocket(listenSocket, WebSocket::State::LISTEN);
    }
    catch (exception& e) {
        cout << e.what() << endl;
        exit(1);
    }
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

bool WebServer::addSocket(SOCKET& id, WebSocket::State state) { //TOOD: is & is needed on socket?
    // Set the socket to be in non-blocking mode.
    unsigned long flag = 1;
    if (ioctlsocket(id, FIONBIO, &flag) != 0)
    {
        string error = "Web Server: Error at ioctlsocket(): " + to_string(WSAGetLastError());
        closesocket(id);
        throw exception(error.c_str());
    }

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
    SOCKET id = sockets[index].getID(); //gets the listener
    struct sockaddr_in from;		    //Address of sending partner
    int fromLen = sizeof(from);

    SOCKET msgSocket = accept(id, (struct sockaddr*)&from, &fromLen);
    if (INVALID_SOCKET == msgSocket)
    {
        string error = "Web Server: Error at accept(): " + to_string(WSAGetLastError());
        throw exception(error.c_str());
    }
    cout << "Web Server: Client " << inet_ntoa(from.sin_addr) << ":" << ntohs(from.sin_port) << " is connected." << endl;

    if (!addSocket(msgSocket, WebSocket::State::RECEIVE))
    {
        closesocket(msgSocket); //TODO: id?
        throw exception("Too many connections, dropped!");
    }
}

void WebServer::receiveMessage(int index) {
    try {
        HttpRequest newReq(sockets[index]);
    }
    catch (exception& e) {
        closesocket(sockets[index].getID()); //TODO: relevant for all the closesockets calls, 
                                             //consider adding this command into the removeSocket func
        removeSocket(index);
        cout << e.what() << endl;
    }
}

void WebServer::sendMessage(int index) {
    try {
        HttpResponse newRes(sockets[index]);
    }
    catch (exception& e) {
        closesocket(sockets[index].getID()); 
        removeSocket(index);
        cout << e.what() << endl;
    }
}

int WebServer::getWaitingSockets()
{
    int nfd;
  
    FD_ZERO(&waitRecv);
    FD_ZERO(&waitSend);

    for (int i = 0; i < MAX_SOCKETS; i++)
    {
        if ((sockets[i].getRecv() == WebSocket::State::LISTEN) || (sockets[i].getRecv() == WebSocket::State::RECEIVE))
            FD_SET(server.getSocket(i).getID(), &waitRecv);
        if (sockets[i].getSend() == WebSocket::State::SEND)
            FD_SET(sockets[i].getID(), &waitSend);
    }

    nfd = select(0, &waitRecv, &waitSend, nullptr, nullptr);
    if (nfd == SOCKET_ERROR)
    {
        string error = "Web Server: Error at select(): " + to_string(WSAGetLastError());
        WSACleanup();
        throw exception(error.c_str());
    }

    return nfd;
}

void WebServer::HandleRecv(int& nfd)
{
    for (int i = 0; i < WebServer::MAX_SOCKETS && nfd > 0; i++)
    {
        if (FD_ISSET(sockets[i].getID(), &waitRecv))
        {
            nfd--;
            switch (sockets[i].getRecv())
            {
            case WebSocket::State::LISTEN:
                try {
                    acceptConnection(i);
                }
                catch (exception& e) {
                    cout << e.what() << endl;
                }
                break;
            case WebSocket::State::RECEIVE:
                receiveMessage(i);
                break;
            default:
                break;
            }
        }
    }
}

void WebServer::HandleSend(int& nfd)
{
    for (int i = 0; i < WebServer::MAX_SOCKETS && nfd > 0; i++)
    {
        if (FD_ISSET(sockets[i].getID(), &waitSend))
        {
            nfd--;
            sendMessage(i);
        }
    }
}
