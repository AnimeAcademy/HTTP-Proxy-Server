# HTTP Proxy Server (C++ / Winsock)

A multithreaded HTTP proxy server written in C++ using the Windows Winsock API. The proxy receives HTTP requests from clients, connects to the requested web server, and forwards the server's response back to the original client.

The project focuses on TCP socket programming, HTTP communication, multithreading, and handling network data.

## Features

- **Multithreaded Client Handling:** Uses `std::thread` to process multiple client connections concurrently.
- **Non-Blocking Server Socket:** Uses `ioctlsocket()` to check for incoming connections without blocking.
- **HTTP Request Forwarding:** Receives HTTP GET requests, identifies the destination using the `Host` header, and forwards the original request.
- **Hostname Resolution:** Supports IPv4 addresses and domain names using `inet_pton()` and `getaddrinfo()`.
- **Response Streaming:** Forwards HTTP responses in chunks instead of storing the entire response in memory.
- **Partial Send Handling:** Accounts for situations where `send()` transmits fewer bytes than requested.
- **Socket Management:** Uses `shutdown()` and `closesocket()` to manage connections and release resources.

## Technologies

- C++
- Windows Winsock2
- TCP/IP and HTTP/1.1
- C++ Standard Library (`std::thread`)
- Visual Studio

## How It Works

1. The proxy listens on a user-specified port at `127.0.0.1`.
2. When a client connects, the server creates a separate thread to handle its request.
3. The client thread receives the HTTP request until the complete headers have arrived.
4. The proxy extracts the destination from the `Host` header and connects to the remote server on port 80.
5. It forwards the original HTTP request.
6. As the response arrives, the proxy forwards it to the client in chunks.
7. Once the response is complete, the connections are closed.

## Building and Running

### Requirements

- Windows
- Visual Studio with C++ development tools
- Winsock2 (included with Windows)

### Build

1. Clone or download the repository.
2. Open the Visual Studio solution (`.sln`).
3. Select your desired build configuration.
4. Build the project.

### Run

Start the executable from a terminal with a port number (adjust the filename if you rename the project):

```powershell
.\CS260_Assignemnt3.exe 8888
```

The server displays:

```text
Listening on port 8888
```

The proxy continues running until manually stopped.

### Testing with curl

```powershell
curl.exe --proxy http://127.0.0.1:8888 http://example.com/
```

## Testing

The proxy was tested using the [FakeHttpClient.NET](https://github.com/DigiPen-Network-Classes/FakeHttpClient.net) testing utility.

Testing included HTTP request forwarding, responses of different sizes, hostname and IPv4 resolution, delayed responses, and concurrent connections. All provided automated tests passed during local testing.

## Limitations

- Supports HTTP traffic on port 80, not HTTPS tunneling.
- Designed for HTTP GET requests without request bodies.
- Uses IPv4 networking.
- Listens only on the local loopback interface.
- Intended as an educational networking project, not a production-ready proxy.

## Project Background

Developed as part of CS260 Computer Networks I at DigiPen Institute of Technology.

The project provided hands-on experience with Windows socket programming, TCP communication, HTTP requests, and managing multiple client connections simultaneously.
