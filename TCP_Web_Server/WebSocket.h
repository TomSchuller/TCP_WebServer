#pragma once
#pragma comment(lib, "Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <iostream>
#include <string>

class WebSocket {
public:
    //Enums
    enum class OperationType { EMPTY = 0, GET, POST, OPTIONS, HEAD, PUT, DEL, TRACE };
    enum class State { EMPTY = 0, LISTEN, RECEIVE, IDLE, SEND };

private:
    SOCKET id;			// Socket handle
    State recv = State::EMPTY;			// Receiving?
    State send = State::IDLE;			// Sending?
    OperationType op = OperationType::EMPTY;	            // Sending sub-type
    std::string asset;

public:
    //Constructor
    WebSocket() = default;

    //dtor
    ~WebSocket() = default;

    //Getters
    const SOCKET& getID() const { return id; }
    const State& getRecv() const { return recv; }
    const State& getSend() const { return send; }
    const OperationType& getOp() const { return op; }
    const std::string& getAsset() const { return asset; }

//Setters
    void setID(SOCKET _id) { id = _id; }
    void setRecv(State _recv) { recv = _recv; }
    void setSend(State _send) { send = _send; }
    void setOp(OperationType _op) { op = _op; }
    void setAsset(std::string _asset) { asset = _asset; }

//Methods
};