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
    catch (WebServerException& e) {
        handleError(listenSocket, e.getMsg(), e.getCode());
        if (e.getCloseSocket()) {
            closesocket(listenSocket);
        }
        cout << e.what() << endl;
        exit(1);
    }
    catch (exception& e) {
        closesocket(listenSocket);
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
        cout << "Web Server: Error at ioctlsocket(): " << to_string(WSAGetLastError());
        throw WebServerException("Internal Server Error", 500);
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
        cout << "Web Server: Error at accept(): " << to_string(WSAGetLastError());
        throw WebServerException("Internal Server Error", 500, false); 
    }
    cout << "Web Server: Client " << inet_ntoa(from.sin_addr) << ":" << ntohs(from.sin_port) << " is connected." << endl;

    if (!addSocket(msgSocket, WebSocket::State::RECEIVE))
    {
        closesocket(msgSocket);
        throw WebServerException("Internal Server Error", 500, false);
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
            try {
                nfd--;
                switch (sockets[i].getRecv())
                {
                case WebSocket::State::LISTEN:
                    acceptConnection(i);
                    break;
                case WebSocket::State::RECEIVE:
                    sockets[i].setRecv(WebSocket::State::IDLE); // TODO: Placed IDLE so we won't recieve same thing from client
                    receiveMessage(i);
                    break;
                default:
                    break;
                }
            }
            catch (WebServerException& e) {
                handleError(sockets[i].getID(), e.getMsg(), e.getCode());
                if (e.getCloseSocket()) {
                    closesocket(sockets[i].getID());
                    removeSocket(i);
                }
                cout << e.what() << endl;
            }
            catch (exception& e) {
                closesocket(sockets[i].getID());
                removeSocket(i);
                cout << e.what() << endl;
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
            try {
                sockets[i].setSend(WebSocket::State::IDLE); // TODO: Placed IDLE so we won't send again to the client
                sendMessage(i);
            }
            catch (WebServerException& e) {
                handleError(sockets[i].getID(), e.getMsg(), e.getCode());
                if (e.getCloseSocket()) {
                    closesocket(sockets[i].getID());
                    removeSocket(i);
                }
                cout << e.what() << endl;
            }
            catch (exception& e) {
                closesocket(sockets[i].getID());
                removeSocket(i);
                cout << e.what() << endl;
            }
        }
    }
}

void WebServer::handleError(const SOCKET& socket, string statusMsg, int statusCode)
{
    string responseMsg;
    string body = "<!DOCTYPE html><html><head><title>Error!</title></head><body><h1>";
    body.append(to_string(statusCode) + " " + statusMsg);
    body.append("</h1></body></html>");
    responseMsg.assign("HTTP/1.1 " + to_string(statusCode) + " " + statusMsg + "\r\n");
    responseMsg.append("Content-Length: " + to_string(body.length()) + "\r\n");
    responseMsg.append("Content-Type: text/html");
    responseMsg.append("\r\n");
    responseMsg.append("\r\n");
    responseMsg.append(body);

    int bytesSent = send(socket, body.c_str(), (int)body.length(), 0);

    if (SOCKET_ERROR == bytesSent)
    {
        string error = "Web Server: Critical Error at Error: " + to_string(statusCode) + "(" + statusMsg + ") Handling: " + to_string(WSAGetLastError());
        cout << error << endl;
    }

    cout << "Web Server: Sent: " << bytesSent << "\\" << body.length() << " bytes." << endl;
}

