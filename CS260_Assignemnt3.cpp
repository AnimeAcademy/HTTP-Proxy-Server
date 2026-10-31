// CS260_Assignemnt3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

using namespace std;
#pragma comment(lib, "ws2_32.lib")

int main(int argc, char* argv[])
{
    if (!argv[1]) {
        cout << "To correctly run this program please pass in a correct port number between 1 and 65535." << endl;
        return 1;
    }
    int port = stoi(argv[1]);
    if (port < 1 || port > 65535) {
        cout << "To correctly run this program please pass in a correct port number between 1 and 65535." << endl;
        return 1;
    }
        u_short PORT = port;

        //Initialize the Library
        WSADATA wsaData;
        int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (res != 0) {
            cout << "WSAStartup:" << WSAGetLastError() << endl;
            return 1;
        }
        //Create a TCP Socket
        SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock == INVALID_SOCKET) {
            cout << "socket: " << WSAGetLastError() << endl;
            return 1;
        }
        //Create Listing Address 

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(PORT);
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

        res = bind(sock, (sockaddr*)&addr, sizeof(addr));
        if (res == SOCKET_ERROR) {
            cout << "bind: " << WSAGetLastError() << endl;
            return 1;
        }
        
        res = listen(sock, SOMAXCONN);
        if (res == SOCKET_ERROR) {
            cout << "listen: " << WSAGetLastError() << endl;
            return 1;
        }

        //Set Socket to non-blocking
        u_long nonBlocking = 1;
        res = ioctlsocket(sock, FIONBIO, &nonBlocking);
        if (res == SOCKET_ERROR) {
            cout << "ioctlsocket: " << WSAGetLastError() << endl;
            return 1;
        }


        sockaddr_in clientAddr{};
        int addrLen = sizeof(clientAddr);

        while (true) {
          
            SOCKET clientSock = accept(sock, (sockaddr*)&clientAddr, &addrLen);
            if (clientSock == INVALID_SOCKET) {
                if(WSAGetLastError() != WSAEWOULDBLOCK)
                    cout << "accept: " << WSAGetLastError() << endl;
                this_thread::sleep_for(chrono::microseconds(100));
            }
            else {
                //run thread

            }


        }




}

void Client(SOCKET clientSock) {
#define BUF_LEN 1400
    char buf[BUF_LEN + 1]; //leave room for \0
    string message;
    int res;
    while (true) {
        res = recv(clientSock, buf, BUF_LEN, 0); // resv = num of bytes recieved
        if (res == SOCKET_ERROR) {
            int error = WSAGetLastError();
            if (error != WSAEWOULDBLOCK) {
                cout << "recv: " << error << endl;
                return;
            }
            else {
                cout << ". " << flush;
                this_thread::sleep_for(chrono::milliseconds(100));
            }
        }
        else if (res == 0) {//done receiving
            break;
        }
        else {//take recieved bytes and store full message then check to see if we have the request
            //buf[res] = '\0';
            message.append(buf, res);
            size_t pos = message.find("\r\n\r\n");
            if (pos != string::npos) {
                //found it and continue with request
                CreateSocket(message);
            }
            //cout << buf << flush;
        }
    }
}

SOCKET CreateSocket(string message) {
    size_t start = message.find("GET ");
    if (start == string::npos) {
        cout << "GET not found" << endl;
        return;
    }

    start += string("GET ").length();
    size_t end = message.find("\r\n", start);
    if (end == string::npos) {
        cout << "End of line not found" << endl;
        return;
    }

    size_t length = end - start;
    string request = message.substr(start, length);
    size_t pos = request.find(' ');

    if (pos != string::npos) {
        string resource = request.substr(0, pos);
        string version = request.substr(pos + 1);
    }

    start = message.find("Host: ");
    if (start == string::npos) {
        cout << "Host: not found" << endl;
        return;
    }

    start += string("Host: ").length();
    end = message.find("\r\n", start);
    if (end == string::npos) {
        cout << "End of line not found" << endl;
        return;
    }

    length = end - start;
    string hostName = message.substr(start, length);




    //Create a TCP Socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        cout << "socket: " << WSAGetLastError() << endl;
        return 1;
    }
    //Create Address Object
    sockaddr_in myAddr; //destination
    memset(myAddr.sin_zero, 0, sizeof(myAddr.sin_zero));
    myAddr.sin_family = AF_INET;
    myAddr.sin_port = htons(80);

    int res = inet_pton(AF_INET, (const char*)&hostName, &myAddr.sin_addr);
    if (res != 1) {
        cout << "inet_pton: " << WSAGetLastError() << endl;
        addrinfo* results = nullptr;
        res = getaddrinfo(hostName.c_str(), "80", nullptr, &results);
    }

    //Connect Socket
    res = connect(sock, reinterpret_cast<sockaddr*>(&myAddr), sizeof(myAddr));
    if (res == SOCKET_ERROR) {
        cout << "connect: " << WSAGetLastError() << endl;
        return 1;
    }
}