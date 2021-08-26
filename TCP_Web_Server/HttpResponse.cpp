#include "HttpResponse.h"

HttpResponse::HttpResponse(WebSocket socket) {
    setStatus(socket.getOp());
    contentType = "text/html";

    string message = createResponse(socket.getAsset());

    SOCKET msgSocket = socket.getID();

    int bytesSent = send(msgSocket, message.c_str(), (int)message.length(), 0);

    if (SOCKET_ERROR == bytesSent)
    {
        cout << "Web Server: Error at send(): " << WSAGetLastError() << endl;
        // TODO: exception
    }

    cout << "Web Server: Sent: " << bytesSent << "\\" << message.length() << " bytes of \"" << message << "\" message.\n";

    socket.setSend(WebSocket::State::IDLE);
}

string HttpResponse::createResponse(string URI) {
    string responseMsg;
    ifstream t(URI);
    string body((std::istreambuf_iterator<char>(t)),
        std::istreambuf_iterator<char>());
    contentLength = to_string(body.length());
    responseMsg.append("HTTP/1.1 200 OK\r\n");
    responseMsg.append("Content-Length: " + contentLength + "\r\n");
    responseMsg.append("Content-Type: " + contentType + "\r\n");
    responseMsg.append("\r\n");
    responseMsg.append(body);
    return responseMsg;
}

void HttpResponse::setStatus(WebSocket::OperationType op) {
    if (op == WebSocket::OperationType::GET) status = "GET";
    if (op == WebSocket::OperationType::POST) status = "POST";
    if (op == WebSocket::OperationType::OPTIONS) status = "OPTIONS";
    if (op == WebSocket::OperationType::HEAD) status = "HEAD";
    if (op == WebSocket::OperationType::PUT) status = "PUT";
    if (op == WebSocket::OperationType::DEL) status = "DEL";
    if (op == WebSocket::OperationType::TRACE) status = "TRACE";
}