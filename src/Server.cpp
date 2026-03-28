#include <iostream>
#include <WS2tcpip.h>
#include <windows.h>
#include <winsock2.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <fstream>
#include "Server.h"
#include "Request/Request.h"
#include "Response/Response.h"
#include <format>
#include <print>
#include <utility>

#pragma comment (lib, "ws2_32.lib");
// HTTP server in c++, I did my own custom implementation

// To compile  "g++ server.cpp -lws2_32 -o server"
int Server::run() {
    // Linking DLL
    // Starting WinSock
    WSADATA wsadata;
    int wsaerr;
    wsaerr = WSAStartup(MAKEWORD(2,2), &wsadata);
    if (wsaerr != 0) {
        std::cout << "Winsock dll not found" << std::endl;
        return 1;
    } else {
        // std::cout << "winsock DLL Found" << std::endl;
        // std::cout << "Status: " << wsadata.szSystemStatus << std::endl;
    }

    // Creating a socket
    SOCKET in = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (in == INVALID_SOCKET) {
        std::cout << "Error at socket()" << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    } else {
        // std::cout << "Socket is OK" << std::endl;
    }

    // SOCKADDR_IN is local endpoint address to 
    // which connect a socket
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_port);
    addr.sin_addr.S_un.S_addr = ADDR_ANY;

    // Binding the socket for the server
    // If bind is okay return 0, else SOCKET_ERROR
    if (bind(in, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cout << "Can't bind socket! " << WSAGetLastError() << std::endl;
		return 1;
    }

    // Listen on port TCP
    if (listen(in, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "error in listen() : " << WSAGetLastError() << std::endl;
    } else {
        std::cout << "Listen() is OK, I'm waiting for connections..." << std::endl;
    }

    while (m_isRunning) {
        // Accept functions
        SOCKET acceptSocket;
        acceptSocket = accept(in, NULL, NULL);
        if(acceptSocket == INVALID_SOCKET) {
            std::cout << "accept failed:" << WSAGetLastError() << std::endl;
            continue;
        } else { 
            std::cout << "accept() is working" << std::endl; 
        } 
        
        // recv() Receives data from the client
        char recvBuf[1024];
        int recvBuflen = sizeof(recvBuf);

        int bytesRecv = recv(acceptSocket, recvBuf, recvBuflen - 1, 0);
        if (bytesRecv > 0) {
            recvBuf[bytesRecv] = '\0';
            
            // Later going to work with threadpool, so every client will have
            // own res req, cause we prevent data race
            Request m_request{};
            Response m_response{};

            // Logging for debug and main logic
            std::cout << "\nRecived from client: \n" << recvBuf << std::endl;

            // send() Send back to the client
            std::string response{};

            const std::string& method { m_request.getMethod(recvBuf) };
            std::println("Method is: {}", method);
            const std::string& path { m_request.getPath(recvBuf) };
            std::println("Path is: {}", path);

            // This parses only headers
            m_request.parser(recvBuf);

            std::string cl { m_request.getHeader("Content-Length") };
            if (!cl.empty()) {
                int clbytes { std::stoi(cl) };

                const int alreayReceived { m_request.getReceivedDataSize() };
                if (alreayReceived > 0) {
                    clbytes -= alreayReceived;
                }

                while (clbytes > 0) {
                    int bytesRecv = recv(acceptSocket, recvBuf, recvBuflen - 1, 0);

                    if (bytesRecv > 0) {
                        recvBuf[bytesRecv] = '\0';
                        std::print("\nRecived from client:\n{}\n\n", recvBuf);
                        m_request.addBody(recvBuf);
                        clbytes -= bytesRecv;
                    } else {
                        break;
                    }
                }
            }
            // The Main logic to response Client
            m_response.findRouteAndExecute(method, path, m_routes, response, m_request, m_response);
            
            // App Logic completes here 


            int bytes_sent = send(acceptSocket, response.c_str(), response.size(), 0);
            closesocket(acceptSocket);

            if (bytes_sent == SOCKET_ERROR) {
                // If sending fails, print an error
                std::cerr << "send failed: " << WSAGetLastError() << std::endl;
            } else {
                // Print the number of bytes sent
                std::cout << "Sent " << bytes_sent << " bytes to client." << std::endl;
            }
            std::println("Connection Closed!");

        } else {
            // If no data is received, print an error message
            std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
        }
    }

    // TODO: Learn how to use signals for graceful shotdown
    // Terminating a Winsock2 dll
    WSACleanup();
    return 0;
}

void Server::Get(const std::string& path, const std::function<void(Request&, Response&)>& lambda) {
    m_routes["GET"][path] = std::make_pair(path, lambda);
}

void Server::Post(const std::string& path, const std::function<void(Request&, Response&)>& lambda) {
    m_routes["POST"][path] = std::make_pair(path, lambda);
}

void Server::Delete(const std::string& path, const std::function<void(Request&, Response&)>& lambda) {
    m_routes["DELETE"][path] = std::make_pair(path, lambda);
}

void Server::Put(const std::string& path, const std::function<void(Request&, Response&)>& lambda) {
    m_routes["PUT"][path] = std::make_pair(path, lambda);
}

void Server::Use(const std::string& path, const std::function<void(Request&, Response&)>& lambda) {
    m_routes["USE"][path] = std::make_pair(path, lambda);
}

// Pretty Cool huh :)

