#define _CRT_SECURE_NO_WARNINGS
#include "WebServer.h"

int main()
{
    WebServer& server = WebServer::getInstance();
    // Accept connections and handles them one by one.
    while (true)
    {
        fd_set waitRecv;
        FD_ZERO(&waitRecv);
        for (int i = 0; i < WebServer::MAX_SOCKETS; i++)
        {
            if ((server.getSocket(i).getRecv() == WebSocket::State::LISTEN) || (server.getSocket(i).getRecv() == WebSocket::State::RECEIVE))
                FD_SET(server.getSocket(i).getID(), &waitRecv);
        }

        fd_set waitSend;
        FD_ZERO(&waitSend);
        for (int i = 0; i < WebServer::MAX_SOCKETS; i++)
        {
            if (server.getSocket(i).getSend() == WebSocket::State::SEND)
                FD_SET(server.getSocket(i).getID(), &waitSend);
        }

        int nfd;
        nfd = select(0, &waitRecv, &waitSend, nullptr, nullptr);
        if (nfd == SOCKET_ERROR)
        {
            cout << "Web Server: Error at select(): " << WSAGetLastError() << endl;
            WSACleanup();
            return 1;
        }

        for (int i = 0; i < WebServer::MAX_SOCKETS && nfd > 0; i++)
        {
            if (FD_ISSET(server.getSocket(i).getID(), &waitRecv))
            {
                nfd--;
                switch (server.getSocket(i).getRecv())
                {
                case WebSocket::State::LISTEN:
                    server.acceptConnection(i);
                    break;

                case WebSocket::State::RECEIVE:
                    server.receiveMessage(i);
                    break;

                default:
                    break;
                }
            }
        }

        for (int i = 0; i < WebServer::MAX_SOCKETS && nfd > 0; i++)
        {
            if (FD_ISSET(server.getSocket(i).getID(), &waitSend))
            {
                nfd--;
                switch (server.getSocket(i).getSend())
                {
                case WebSocket::State::SEND:
                    server.sendMessage(i);
                    break;

                default:
                    break;
                }
            }
        }
    }
}