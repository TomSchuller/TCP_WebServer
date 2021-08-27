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


            // Set the socket to be in non-blocking mode.
            unsigned long flag = 1;
            if (ioctlsocket(sockets[i].getID(), FIONBIO, &flag) != 0)
            {
                cout << "Web Server: Error at ioctlsocket(): " << WSAGetLastError() << endl;
                //TODO: exception
            }
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
        cout << "Web Server: Error at accept(): " << WSAGetLastError() << endl;
        //TODO: exception
    }
    cout << "Web Server: Client " << inet_ntoa(from.sin_addr) << ":" << ntohs(from.sin_port) << " is connected." << endl;

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

int WebServer::getWaitingSockets()
{
    int nfd;
  
    FD_ZERO(&waitRecv);
    FD_ZERO(&waitSend);

    for (int i = 0; i < MAX_SOCKETS; i++)
    {
        if ((sockets[i].getRecv() == WebSocket::State::LISTEN) || (sockets[i].getRecv() == WebSocket::State::RECEIVE))
            FD_SET(server.getSocket(i).getID(), &waitRecv);
    }

    for (int i = 0; i < WebServer::MAX_SOCKETS; i++)
    {
        if (sockets[i].getSend() == WebSocket::State::SEND)
            FD_SET(sockets[i].getID(), &waitSend);
    }

    nfd = select(0, &waitRecv, &waitSend, nullptr, nullptr);
    if (nfd == SOCKET_ERROR)
    {
        cout << "Web Server: Error at select(): " << WSAGetLastError() << endl;
        WSACleanup();
        //TODO: exception
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
                acceptConnection(i);
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
