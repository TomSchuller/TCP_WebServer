#include "HttpRequest.h"

HttpRequest::HttpRequest(WebSocket& socket)  {
    SOCKET msgSocket = socket.getID();
    char* buffer = new char[MAX_RECV_BUFF + 1];

    int bytesRecv = recv(msgSocket, buffer, MAX_RECV_BUFF, 0);

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
        string recvBuffer;
        buffer[bytesRecv] = '\0'; //add the null-terminating to make it a string
        recvBuffer.assign(buffer);

        cout << "Web Server Received: " << bytesRecv << " bytes." << endl;// of " << recvBuffer << " message." << endl;

        operation = parseOperation(recvBuffer);
        lang = parseLang(recvBuffer); //TODO: Fix a bug that appears when there isn't a qurrey string
        uri = parseURI(recvBuffer);

        socket.setAsset(uri);
        socket.setOp(operation);
        socket.setSend(WebSocket::State::SEND);
    }
}

string HttpRequest::parseURI(string& buffer) {
    string _uri;
    istringstream iss(buffer);
    getline(iss, _uri, ' ');
    getline(iss, _uri, ' ');
    return _uri;
}

string HttpRequest::parseLang(string& buffer) {
    string _lang;
    istringstream iss(buffer);
    getline(iss, _lang, '?');
    getline(iss, _lang, '=');
    _lang = _lang.substr(0, 2);
    return _lang;
}

WebSocket::OperationType HttpRequest::parseOperation(string& buffer) {
    string op;
    istringstream iss(buffer);
    getline(iss, op, ' ');
    if (op == "GET") return WebSocket::OperationType::GET;
    if (op == "POST") return WebSocket::OperationType::POST;
    if (op == "OPTIONS") return WebSocket::OperationType::OPTIONS;
    if (op == "HEAD") return WebSocket::OperationType::HEAD;
    if (op == "PUT") return WebSocket::OperationType::PUT;
    if (op == "DELETE") return WebSocket::OperationType::DEL;
    if (op == "TRACE") return WebSocket::OperationType::TRACE;
    else return WebSocket::OperationType::EMPTY;
}
