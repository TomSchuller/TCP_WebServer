/////////////////////////////////
// This Class defines a Web socket.
/////////////////////////////////
#pragma once
#pragma comment(lib, "Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <iostream>
#include <string>

using namespace std;

class WebSocket {
public:
    //Enums
    enum class OperationType { EMPTY = 0, GET, POST, OPTIONS, HEAD, PUT, DEL, TRACE };
    enum class State { EMPTY = 0, LISTEN, RECEIVE, IDLE, SEND };

private:
    SOCKET id;			// Socket handle
    State recv = State::EMPTY;			// Receiving?
    State send = State::IDLE;			// Sending?
    string request;

public:
    //Constructor
    WebSocket() = default;

    //dtor
    ~WebSocket() = default;

    //Getters
    const SOCKET& getID() const { return id; }
    const State& getRecv() const { return recv; }
    const State& getSend() const { return send; }
    const string& getRequest() const { return request; }

    //Setters
    void setID(SOCKET _id) { id = _id; }
    void setRecv(State _recv) { recv = _recv; }
    void setSend(State _send) { send = _send; }
    void setRequest(string _request) { request = _request; }
};