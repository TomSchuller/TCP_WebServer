/********************************
 * Name: Guy Podolsky, I.D: 206799199
 * Name: Tom Schuller, I.D: 205928146
 * Date: 03/09/2021
 * Computer Networkings - TCP Web Server Excercise
 * This program contains functions that find that define a TCP web server.
 ********************************/

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
        catch (WebServerException& e) {
            cout << e.what() << endl;
            exit(1);
        }
        catch (exception& e) {
            cout << e.what() << endl;
            exit(1);
        }
    }
}