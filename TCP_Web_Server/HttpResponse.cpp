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
        cout << "Web Server: Error at send(): " << to_string(WSAGetLastError());
        throw WebServerException("Internal Server Error", 500, false);

    }

    cout << "Web Server: Sent: " << bytesSent << "\\" << message.length() << " bytes." << endl; // of \"" << message << "\" message.\n";

    socket.setSend(WebSocket::State::IDLE);
}

string HttpResponse::createResponse(WebSocket& socket) {
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
        throw WebServerException("Method Not Allowed", 405, false);
        break;
    }
}

string HttpResponse::doGET(WebSocket& socket)
{
    string responseMsg;
    stringstream body;
    string fullAddr;
    string defualtFullAddr;
    string addr = uri;

    //if there is no path, it sets a default one
    if (uri == "/") { addr = "/index.html"; }
   
    fullAddr = defualtFullAddr = "www";
    defualtFullAddr += ("/default"+addr);

    if (lang != "") { fullAddr += ("/" + lang); }
    else { fullAddr += "/default"; }
    fullAddr +=addr;

    statusCode = "200";
    statusMsg = "OK";

    ifstream r1(fullAddr, ios::in);
    ifstream r2(defualtFullAddr, ios::in);
    if (r1.is_open()) { body << r1.rdbuf(); }
    else if (r2.is_open()) { body << r2.rdbuf(); }
    else {
        r1.close();
        r2.close();
        throw WebServerException("Not Found", 404, false);
    }
    r1.close();
    r2.close();

    contentLength = to_string(body.str().length());
    responseMsg.append("HTTP/1.1 " + statusCode + " " + statusMsg + "\r\n");
    if (lang != "") {
        responseMsg.append("Content-Language: " + lang + "\r\n");
    }
    responseMsg.append("Content-Length: " + contentLength + "\r\n");
    responseMsg.append("Content-Type: " + contentType + "\r\n");
    responseMsg.append("\r\n");
    responseMsg.append(body.str());
    return responseMsg;
}

string HttpResponse::doPOST(WebSocket& socket)
{
    string responseMsg;
    string addr = uri;

    //if there is no path, it sets a default one
    if (uri == "/") { addr = "/index.html"; }
    string fullAddr = "www/default" + addr;

    ifstream check(fullAddr, ios::in); //checks if exists
    if (!check.is_open()) {
        statusCode = "201";
        statusMsg = "Created";
        ofstream t(fullAddr, ofstream::trunc); //sets the new resource
        if (!t.is_open()) {
            t.close();
            throw WebServerException("Internal Server Error", 500, false);
        }
        t.close();
    }
    else {
        statusCode = "204";
        statusMsg = "No Content";
    }
    check.close();

    cout << parseBody(socket.getRequest());
    

    responseMsg.append("HTTP/1.1 " + statusCode + " " + statusMsg + "\r\n");
    responseMsg.append("\r\n ");
    return responseMsg;
}

string HttpResponse::doOPTIONS(WebSocket& socket)
{
    string responseMsg, Addr = uri;
    stringstream body;

    //if there is no path, it sets a default one
    if (uri == "/" || uri == "*") { Addr = "/index.html"; }
    string fullAddr = "www/default" + Addr;

    ifstream t(fullAddr, ios::in);
    if (t.is_open()) {
        statusCode = "204";
        statusMsg = "No Content";
        body << "Allow: OPTIONS,GET,HEAD,POST,DELETE,TRACE,PUT\r\n";
    }
    else {
        t.close();
        throw WebServerException("Not Found", 404, false);
    }

    t.close();
    responseMsg.append("HTTP/1.1 " + statusCode + " " + statusMsg + "\r\n");
    responseMsg.append(body.str());
    responseMsg.append("\r\n");
    return responseMsg;
}

string HttpResponse::doPUT(WebSocket& socket)
{
    string responseMsg, addr = uri, body = parseBody(socket.getRequest());

    //if there is no path, it sets a default one
    if (uri == "/") { addr = "/index.html"; }
    string fullAddr = "www/default" + addr;

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
        t.close();
        throw WebServerException("Internal Server Error", 500, false);
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

    //if there is no path, it sets a default one
    if (uri == "/") { addr = "/index.html"; }
    string fullAddr = "www/default" + addr;
   
    ifstream t(fullAddr.c_str());
    if (!(bool)t) { throw WebServerException("Not Found", 404, false); }
    else { t.close(); }
   
    if (remove(fullAddr.c_str()) != 0) { throw WebServerException("Internal Server Error", 500, false); }
    else {
        statusCode = "204";
        statusMsg = "No Content";
        responseMsg.append("HTTP/1.1 " + statusCode + " " + statusMsg + "\r\n");
        responseMsg.append("\r\n");
    }
    return responseMsg;
}

string HttpResponse::doTRACE(WebSocket& socket)
{
    string responseMsg;
    stringstream body;
    string addr = uri;

    //if there is no path, it sets a default one
    if (uri == "/") { addr = "/index.html"; }
    string fullAddr = "www/default" + addr;

    ifstream t(fullAddr, ios::in);
    if (t.is_open()) {
        statusCode = "200";
        statusMsg = "OK";
        contentType = "message/http";
        body << socket.getRequest();
    }
    else {
        t.close();
        throw WebServerException("Not Found", 404, false);
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

string HttpResponse::doHEAD(WebSocket& socket)
{
    string responseMsg;
    stringstream body;
    string addr = uri;

    //if there is no path, it sets a default one
    if (uri == "/") { addr = "/index.html"; }
    string fullAddr = "www/default" + addr;

    ifstream t(fullAddr, ios::in);
    if (t.is_open()) {
        statusCode = "204";
        statusMsg = "No Content";
        body << t.rdbuf();
    }
    else {
        t.close();
        throw WebServerException("Not Found", 404, false);
    }
    t.close();

    contentLength = to_string(body.str().length());
    responseMsg.append("HTTP/1.1 " + statusCode + " " + statusMsg + "\r\n");
    responseMsg.append("Content-Length: " + contentLength + "\r\n");
    responseMsg.append("Content-Type: " + contentType + "\r\n");
    responseMsg.append("\r\n");

    return responseMsg;
}

string HttpResponse::parseURI(const string& buffer) {
    string _uri;
    istringstream iss(buffer);
    getline(iss, _uri, ' ');
    getline(iss, _uri, ' ');
    int pos = _uri.find("?");
    if (pos != EOF)
    {
        _uri = _uri.substr(0, pos);
    }
    return _uri;
}

string HttpResponse::parseBody(const string& buffer)
{
    string phrase = "\r\n\r\n";
    int pos = buffer.find(phrase);
    if (pos == EOF) {
        throw WebServerException("Bad Request", 400, false);

    }
    return buffer.substr(pos+phrase.length());
}

string HttpResponse::parseLang(const string& buffer) {
    string query;
    string hasQuery = "?";
    string hasLangQuery = "lang=";

    string _uri;
    istringstream iss(buffer);
    getline(iss, _uri, ' '); //will get http/1.1
    getline(iss, _uri, ' '); //will get the uri

    int pos = _uri.find(hasQuery); //checks if there is a query
    if (pos != EOF) {
        query = _uri.substr(pos + hasQuery.length());
        pos = query.find(hasLangQuery); //checks if this is a lang query
        if (pos == EOF || (query.length() - (pos + hasLangQuery.length()) < 2)) {
            throw WebServerException("Bad Request", 400, false);
        }
        return query.substr(pos + hasLangQuery.length(),2);
    }
  return "";
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