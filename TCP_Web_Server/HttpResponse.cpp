#define _CRT_SECURE_NO_WARNINGS
#include "HttpResponse.h"

HttpResponse::HttpResponse(WebSocket& socket) {
    setStatus(socket.getOp());
    contentType = "text/html";

    string message = createResponse(socket.getAsset());

    SOCKET msgSocket = socket.getID();

    int bytesSent = send(msgSocket, message.c_str(), (int)message.length(), 0);

    if (SOCKET_ERROR == bytesSent)
    {
        string error = "Web Server: Error at send(): " + to_string(WSAGetLastError());
        throw exception(error.c_str());
    }

    cout << "Web Server: Sent: " << bytesSent << "\\" << message.length() << " bytes." << endl; // of \"" << message << "\" message.\n";

    socket.setSend(WebSocket::State::IDLE);
}

string HttpResponse::createResponse(string URI) {
    
   
}

string HttpResponse::doGET(string URI)
{
    string responseMsg;
    stringstream body;

    if (URI == "/") { URI = "/index.html"; }
    string fullAddr = "www" + URI;

    ifstream t(fullAddr, ios::in);
    if (t.is_open()) { body << t.rdbuf(); }

    contentLength = to_string(body.str().length());
    responseMsg.append("HTTP/1.1 200 OK\r\n");
    responseMsg.append("Content-Length: " + contentLength + "\r\n");
    responseMsg.append("Content-Type: " + contentType + "\r\n");
    responseMsg.append("\r\n");
    responseMsg.append(body.str());
    return responseMsg;
}

void HttpResponse::setStatus(WebSocket::OperationType op) {
    if (op == WebSocket::OperationType::GET) status = "GET";
    if (op == WebSocket::OperationType::POST) status = "POST";
    if (op == WebSocket::OperationType::OPTIONS) status = "OPTIONS";
    if (op == WebSocket::OperationType::HEAD) status = "HEAD";
    if (op == WebSocket::OperationType::PUT) status = "PUT";
    if (op == WebSocket::OperationType::DEL) status = "DELETE";
    if (op == WebSocket::OperationType::TRACE) status = "TRACE";
    else status = "EMPTY";
}