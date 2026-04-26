#include <iostream>
#include <WS2tcpip.h>
#include <windows.h>
#include <winsock2.h>
#include <stdio.h>
#include <string>
#include "Server.h"
#include "Request/Request.h"
#include "Response/Response.h"
#include <print>
#include <utility>
#include <csignal>
#include <thread>
#include <chrono>
#include <condition_variable>

// HTTP server in c++, I did my own custom implementation

// Local globals and Declarations
static bool glocal_isRunning{ true };
std::condition_variable glocal_cv{};
void signal_handler(int signal);

int Server::run() {
    
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2,2), &wsadata) != 0) {
        std::cout << "Winsock dll not found" << std::endl;
        return 1;
    }

    SOCKET in = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (in == INVALID_SOCKET) {
        std::cout << "Error at socket()" << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_port);
    addr.sin_addr.S_un.S_addr = ADDR_ANY;

    if (bind(in, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cout << "Can't bind socket! " << WSAGetLastError() << std::endl;
        WSACleanup();
		return 1;
    }

    if (listen(in, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "error in listen() : " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    } else {
        std::cout << "Listen() is OK, I'm waiting for connections..." << std::endl;
    }

    std::signal(SIGINT, signal_handler);
    std::thread closeServerClientSOCK([&in, this](){
        std::unique_lock<std::mutex> lock(m_mutex);
        glocal_cv.wait(lock, []{ return !glocal_isRunning; });
        closesocket(in);
        if (m_clientSocket != INVALID_SOCKET) {
            closesocket(m_clientSocket);
        }
    });
    closeServerClientSOCK.detach();

    while (glocal_isRunning) {
        // Accept functions
        m_clientSocket = accept(in, NULL, NULL);
        if(m_clientSocket == INVALID_SOCKET) {
            std::cout << "accept failed:" << WSAGetLastError() << std::endl;
            if (!glocal_isRunning) {
                std::println("Socket Connection was Closed Successfully!");
                break;
            }
        } else { 
            std::cout << "accept() is working" << std::endl; 
        }
        
        // recv() Receives data from the client
        char recvBuf[1024];
        int recvBuflen = sizeof(recvBuf);

        int bytesRecv = recv(m_clientSocket, recvBuf, recvBuflen - 1, 0);
        if (bytesRecv > 0) {
            auto start { std::chrono::steady_clock::now() };
            recvBuf[bytesRecv] = '\0';
            
            // Later going to work with threadpool, so every client will have
            // own res req, cause we prevent data race
            Request m_request{};
            Response m_response{};

            // Logging for debug and main logic
            std::println("\nRecived from client:\n{}", recvBuf);

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
                    int bytesRecv = recv(m_clientSocket, recvBuf, recvBuflen - 1, 0);

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

            int bytes_sent = send(m_clientSocket, response.c_str(), response.size(), 0);
            closesocket(m_clientSocket);

            if (bytes_sent == SOCKET_ERROR) {
                // If sending fails, print an error
                std::cerr << "send failed: " << WSAGetLastError() << std::endl;
            } else {
                // Print the number of bytes sent
                std::cout << "Sent " << bytes_sent << " bytes to client." << std::endl;
            }
            std::println("Connection Closed!");

            auto end { std::chrono::steady_clock::now() };
            auto duration {std::chrono::duration<double, std::milli>(end - start)};
            std::println("Time used WHOLE request: {}", duration);
            
        } else {
            // If no data is received, print an error message
            std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
        }
    }

    std::println("Graceful Shotdown!");
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

void Server::Options(const std::string& path, const std::function<void(Request&, Response&)>& lambda) {
    m_routes["OPTIONS"][path] = std::make_pair(path, lambda);
}

void Server::Use(const std::string& path, const std::function<void(Request&, Response&)>& lambda) {
    m_routes["USE"][path] = std::make_pair(path, lambda);
}

void signal_handler(int signal) {
    if (signal == SIGINT) {
        glocal_isRunning = false;
        glocal_cv.notify_all();
    }
}


// Pretty Cool huh :)

