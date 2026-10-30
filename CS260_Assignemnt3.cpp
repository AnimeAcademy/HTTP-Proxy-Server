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

        int res = bind(sock, (sockaddr*)&addr, sizeof(addr));
        if (res == SOCKET_ERROR) {
            cout << "bind: " << WSAGetLastError() << endl;
        }
        
        res = listen(sock, SOMAXCONN);
        if (res == SOCKET_ERROR) {
            cout << "listen: " << WSAGetLastError() << endl;
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
            }
            else {

            }


        }
















        














}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
