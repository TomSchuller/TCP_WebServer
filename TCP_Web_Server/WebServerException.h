/////////////////////////////////
// This Class extends an excpetion class.
/////////////////////////////////
#pragma once
#include <string>

using namespace std;


class WebServerException : public exception {
private:
    int statusCode;
    bool closeSocket;
    string statusMsg;
    string whatMsg;
public:
    WebServerException(string _msg, int _status, bool _closeSocket) : statusMsg(_msg), statusCode(_status), closeSocket(_closeSocket) {
        whatMsg.append(to_string(_status));
        whatMsg.append(" " + _msg);
    }

    WebServerException(string _msg, int _status) : statusMsg(_msg), statusCode(_status), closeSocket(true) {
        whatMsg.append(to_string(_status));
        whatMsg.append(" " + _msg);
    }

    const char* what() const throw() {
        return whatMsg.c_str();
    }

    const string& getMsg() const { return statusMsg; }
    const int getCode() const { return statusCode;  }
    const bool getCloseSocket() const { return closeSocket; }
};