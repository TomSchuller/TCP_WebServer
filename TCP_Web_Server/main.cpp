#define _CRT_SECURE_NO_WARNINGS
#include "WebServer.h"

int main()
{
    WebServer& server = WebServer::getInstance();
    // Accept connections and handles them one by one.
    while (true)
    {
        try {
            int NumOfWaitingSockets = server.getWaitingSockets();
            server.HandleSend(NumOfWaitingSockets); 
            server.HandleRecv(NumOfWaitingSockets);
        }
        catch (exception& e) {
            cout << e.what() << endl;
            exit(1);
        }
    }
}