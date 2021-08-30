#define _CRT_SECURE_NO_WARNINGS
#include "HttpResponse.h"

HttpResponse::HttpResponse(WebSocket& socket) {
    operation = parseOperation(socket.getRequest());
    uri = parseURI(socket.getRequest());
    lang = parseLang(socket.getRequest());

    contentType = "text/html";

    string message = createResponse(socket);

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

string HttpResponse::createResponse(WebSocket& socket) {
    //TODO: Add language support
    switch (operation) {
    case WebSocket::OperationType::GET:
        return doGET(socket);
        break;
    case WebSocket::OperationType::POST:
        return doPOST(socket);
        break;
    case WebSocket::OperationType::OPTIONS:
        return doOPTIONS(socket);
        break;
    case WebSocket::OperationType::DEL:
        return doDELETE(socket);
        break;
    case WebSocket::OperationType::PUT:
        return doPUT(socket);
        break;
    case WebSocket::OperationType::TRACE:
        return doTRACE(socket);
        break;
    case WebSocket::OperationType::HEAD:
        return doHEAD(socket);
        break;
    default:
        return doERROR();
        break;
    }
}

string HttpResponse::doGET(WebSocket& socket)
{
    string responseMsg;
    stringstream body;
    string addr = uri;

    if (uri == "/") { addr = "/index.html"; }
    string fullAddr = "www" + addr;

    ifstream t(fullAddr, ios::in);
    if (t.is_open()) {
        statusCode = "200";
        statusMsg = "OK";
        body << t.rdbuf();
    }
    else {
        statusCode = "404";
        statusMsg = "Not Found";
        body << "";
    }
    t.close();

    contentLength = to_string(body.str().length());
    responseMsg.append("HTTP/1.1 " + statusCode + " " + statusMsg + "\r\n");
    responseMsg.append("Content-Length: " + contentLength + "\r\n");
    responseMsg.append("Content-Type: " + contentType + "\r\n");
    responseMsg.append("\r\n");
    responseMsg.append(body.str());
    return responseMsg;
}

string HttpResponse::doPOST(WebSocket& socket)
{
    //TODO: how to deal with the unlimited size of post requests.
    return string();
}

string HttpResponse::doOPTIONS(WebSocket& socket)
{
    string responseMsg, Addr;
    stringstream body;

    if (uri == "/" || uri == "*") { Addr = "/index.html"; }
    string fullAddr = "www" + Addr;

    ifstream t(fullAddr, ios::in);
    if (t.is_open()) {
        statusCode = "204";
        statusMsg = "No Content";
        body << "Allow: OPTIONS,GET,HEAD,POST,DELETE,TRACE,PUT\r\n";
    }
    else {
        statusCode = "404";
        statusMsg = "Not Found";
        body << "";
    }

    t.close();
    responseMsg.append("HTTP/1.1 " + statusCode + " " + statusMsg + "\r\n");
    responseMsg.append(body.str());
    responseMsg.append("\r\n");
    return responseMsg;
}

string HttpResponse::doPUT(WebSocket& socket) // TODO: Change body
{
    string responseMsg, addr = uri, body = parseBody(socket.getRequest());

    if (uri == "/") { addr = "/index.html"; }
    string fullAddr = "www" + addr;

    ifstream check(fullAddr, ios::in); //checks if exists
    if (!check.is_open()) {
        statusCode = "201";
        statusMsg = "Created";
    }
    else {
        statusCode = "200";
        statusMsg = "OK";
    }
    check.close();

    ofstream t(fullAddr, ofstream::trunc); //sets the new resource
    if (t.is_open()) {
        t << body;
    }
    else {
        statusCode = "500";
        statusMsg = "Internal Server Error";
    }
    t.close();

    contentLength = to_string(body.length());
    responseMsg.append("HTTP/1.1 " + statusCode + " " + statusMsg + "\r\n");
    responseMsg.append("Content-Length: " + contentLength + "\r\n");
    responseMsg.append("Content-Type: " + contentType + "\r\n");
    responseMsg.append("\r\n");
    responseMsg.append(body);
    return responseMsg;
}


string HttpResponse::doDELETE(WebSocket& socket)
{
    string responseMsg, addr = uri, body = parseBody(socket.getRequest());

    if (uri == "/") { addr = "/index.html"; }
    string fullAddr = "www" + addr;

    if (remove(fullAddr.c_str()) != 0) { // TODO: Change body
        statusCode = "500";
        statusMsg = "Internal Server Error";
        body = "<h3>Error 500 Internal Server Error</h3>";
        contentLength = to_string(body.length());
        responseMsg.append("HTTP/1.1 " + statusCode + " " + statusMsg + "\r\n");
        responseMsg.append("Content-Length: " + contentLength + "\r\n");
        responseMsg.append("Content-Type: " + contentType + "\r\n");
        responseMsg.append("\r\n");
        responseMsg.append(body);
    }
    else {
        statusCode = "204";
        statusMsg = "No Content";
        responseMsg.append("HTTP/1.1 " + statusCode + " " + statusMsg + "\r\n");
        responseMsg.append("\r\n");
    }
    responseMsg.append("HTTP/1.1 " + statusCode + " " + statusMsg + "\r\n");
    responseMsg.append("\r\n");
    return responseMsg;
}

string HttpResponse::doTRACE(WebSocket& socket)
{
    return string();
}

string HttpResponse::doHEAD(WebSocket& socket)
{
    string responseMsg;
    stringstream body;
    string addr = uri;

    if (uri == "/") { addr = "/index.html"; }
    string fullAddr = "www" + addr;

    ifstream t(fullAddr, ios::in);
    if (t.is_open()) {
        statusCode = "204";
        statusMsg = "No Content";
        body << t.rdbuf();
    }
    else {
        statusCode = "404";
        statusMsg = "Not Found";
        body << "";
    }
    t.close();

    contentLength = to_string(body.str().length());
    responseMsg.append("HTTP/1.1 " + statusCode + " " + statusMsg + "\r\n");
    responseMsg.append("Content-Length: " + contentLength + "\r\n");
    responseMsg.append("Content-Type: " + contentType + "\r\n");
    responseMsg.append("\r\n");

    return responseMsg;
}

string HttpResponse::doERROR()
{
    return string();
}

string HttpResponse::parseURI(const string& buffer) {
    string _uri;
    istringstream iss(buffer);
    getline(iss, _uri, ' ');
    getline(iss, _uri, ' ');
    return _uri;
}

string HttpResponse::parseBody(const string& buffer)
{
    string phrase = "\r\n\r\n";
    int pos = buffer.find_last_of(phrase);
    if (pos == EOF) {
        throw exception("Bad Requset", 400);
    }
    return buffer.substr(++pos);
}

string HttpResponse::parseLang(const string& buffer) {
    string _lang;
    istringstream iss(buffer);
    getline(iss, _lang, '?');
    getline(iss, _lang, '=');
    _lang = _lang.substr(0, 2);
    return _lang;
}

WebSocket::OperationType HttpResponse::parseOperation(const string& buffer) {
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