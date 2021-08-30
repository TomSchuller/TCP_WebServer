#include "HttpRequest.h"

HttpRequest::HttpRequest(WebSocket& socket)  {
    SOCKET msgSocket = socket.getID();
    char* bufferStr = new char[MAX_RECV_BUFF + 1];

    int bytesRecv = recv(msgSocket, bufferStr, MAX_RECV_BUFF, 0);

    if (SOCKET_ERROR == bytesRecv)
    {
        string error = "Web Server: Error at recv(): " + to_string(WSAGetLastError());
        throw exception(error.c_str());
    }
    if (bytesRecv <= 0)
    {
        string error = "Web Server: Close connection request was recived from socket: " + to_string(msgSocket);
        throw exception(error.c_str());
    }
    else
    {
        bufferStr[bytesRecv] = '\0'; //add the null-terminating to make it a string
        buffer.assign(bufferStr);

        cout << "Web Server Received: " << bytesRecv << " bytes." << endl;// of " << recvBuffer << " message." << endl;

        socket.setRequest(buffer);
        socket.setSend(WebSocket::State::SEND);
    }
}


