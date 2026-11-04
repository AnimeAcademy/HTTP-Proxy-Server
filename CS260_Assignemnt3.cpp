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
                this_thread::sleep_for(chrono::milliseconds(100));
            }
            else {
                //run thread
                thread worker(Client, clientSock);
                Worker.detatch();
            }


        }


    //Shutting Down Library
    res = WSACleanup();
    if (res != 0) {
        cout << "WSACleanup: " << WSAGetLastError() << endl;
        return 1;
    }

}

void Client(SOCKET clientSock) {
#define BUF_LEN 1400
    char buf[BUF_LEN + 1]; //leave room for \0
    string message;
    SOCKET sock = INVALID_SOCKET;
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
            message.append(buf, res);
            size_t pos = message.find("\r\n\r\n");
            if (pos != string::npos) {
                //found it and continue with request
                sock = CreateSocketFromMessageRequest(message);
                if (sock == INVALID_SOCKET) {
                    return;
                }
                break;
            }

        }
    }

    //Send request to server
    int totalBytes = (int)message.length();
    int bytesSent = 0;
    while (bytesSent != totalBytes) {
        res = send(sock, (message.c_str() + bytesSent), (totalBytes - bytesSent), 0);
        if (res == SOCKET_ERROR) {
            if (WSAGetLastError() != WSAEWOULDBLOCK) {
                cout << "send: " << WSAGetLastError() << endl;
                return;
            }
            this_thread::sleep_for(chrono::milliseconds(100));
        }
        else
            bytesSent += res;
    }

    //Tell Server I'm done sending
    res = shutdown(sock, SD_SEND);
    if (res == SOCKET_ERROR) {
        cout << "shutdown: " << WSAGetLastError() << endl;
        return;
    }


    //recieve data requested from server and close socket
    totalBytes = sizeof(buf);
    int bytesRecieved = 0;
    while (bytesRecieved != totalBytes) {
        res = recv(sock,(buf + bytesRecieved), (BUF_LEN- bytesRecieved), 0);
        if (res == SOCKET_ERROR) {
            cout << "recv: " << WSAGetLastError() << endl;
            return;
        }
        if (res == 0) {
            buf[bytesRecieved] = '\0';
            break;
        }
        bytesRecieved += res;
    }
    //Close Server Socket
    res = closesocket(sock);
    if (res != 0) {
        cout << "closesocket: " << WSAGetLastError() << endl;
        return;
    }


    //send data back to client
    totalBytes = bytesRecieved;
    bytesSent = 0;
    while (bytesSent < bytesRecieved) {
        res = send(clientSock, (buf + bytesSent), (totalBytes - bytesSent), 0);
        if (res == SOCKET_ERROR) {
            if (WSAGetLastError() != WSAEWOULDBLOCK) {
                cout << "send: " << WSAGetLastError() << endl;
                return;
            }
            this_thread::sleep_for(chrono::milliseconds(100));
        }
        else
            bytesSent += res;
    }

    //Tell Client I'm done sending
    res = shutdown(clientSock, SD_SEND);
    if (res == SOCKET_ERROR) {
        cout << "shutdown: " << WSAGetLastError() << endl;
        return;
    }

    //Close client Socket
    res = closesocket(clientSock);
    if (res != 0) {
        cout << "closesocket: " << WSAGetLastError() << endl;
        return;
    }
}

SOCKET CreateSocketFromMessageRequest(string message) {
    size_t start = message.find("Host: ");
    if (start == string::npos) {
        cout << "Host: not found" << endl;
        return INVALID_SOCKET;
    }

    start += string("Host: ").length();
    size_t end = message.find("\r\n", start);
    if (end == string::npos) {
        cout << "End of line not found" << endl;
        return INVALID_SOCKET;
    }

    size_t length = end - start;
    string hostName = message.substr(start, length);


    //Create Address Object
    sockaddr_in myAddr; //destination
    memset(myAddr.sin_zero, 0, sizeof(myAddr.sin_zero));
    myAddr.sin_family = AF_INET;
    myAddr.sin_port = htons(80);

    SOCKET sock = INVALID_SOCKET;
    int res = inet_pton(AF_INET, hostName.c_str(), &myAddr.sin_addr);
    if (res == 0) {

        addrinfo hints{};
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        addrinfo* results = nullptr;
        res = getaddrinfo(hostName.c_str(), "80", &hints, &results);
        if (res != 0) {
            cout << "getaddrinfo failed: " << res << endl;
            return INVALID_SOCKET;
        }
        for (addrinfo* current = results;
            current != nullptr;
            current = current->ai_next) {

            SOCKET sok = socket(current->ai_family, current->ai_socktype, current->ai_protocol);
            if (sok == INVALID_SOCKET) {
                cout << "socket: " << WSAGetLastError() << endl;
                continue;
            }
            else {
                res = connect(sok, current->ai_addr, current->ai_addrlen);
                if (res == SOCKET_ERROR) {
                    cout << "connect: " << WSAGetLastError() << endl;
                    closesocket(sok);
                    continue;
                }
                else {
                    sock = sok;
                    break;
                }
            }
        }
        freeaddrinfo(results);
    }
    else if (res == 1) {//if inet worked
        //Create a TCP Socket
        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock == INVALID_SOCKET) {
            cout << "socket: " << WSAGetLastError() << endl;
            return INVALID_SOCKET;
        }
        //Connect Socket
        res = connect(sock, reinterpret_cast<sockaddr*>(&myAddr), sizeof(myAddr));
        if (res == SOCKET_ERROR) {
            cout << "connect: " << WSAGetLastError() << endl;
            return INVALID_SOCKET;
        }
    }
    else {
        cout << "inet_pton: " << WSAGetLastError() << endl;
        return INVALID_SOCKET;
    }

    return sock;
}