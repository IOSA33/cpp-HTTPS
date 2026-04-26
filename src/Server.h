#pragma once

#include <string>
#include <functional>
#include <map>
#include <print>
#include <utility>
#include <winsock2.h>
#include <mutex>
#include "Request/Request.h"
#include "Response/Response.h"

namespace Middleware {
    enum Code {
        PageNotFound,
        MaxCodes
    };
}

class Server {
private:
    int m_port{};
    std::string m_ip{};
    std::string m_certificate{};
    std::string m_private_key{};
    SOCKET m_clientSocket{};
    std::mutex m_mutex{};
    // Method, route, origPath, lambda
    std::map<std::string, std::map<std::string, std::pair<std::string, std::function<void(Request&, Response&)>>>> m_routes;
    
public:
    Server(const std::string& ip, int port) 
        : m_port(port), m_ip(ip) {
            std::println("Server Started at Port: {}", port);
        }
    Server(const std::string& ip, int port, const std::string& certificate, const std::string& private_key)
        : m_port(port), m_ip(ip), m_certificate(certificate), m_private_key(private_key) {
            std::println("(TLS) Server Started at Port: {}", port);
        }
    int run();

    // Methods
    void Get(const std::string& route, const std::function<void(Request&, Response&)>& lambda);
    void Post(const std::string& route, const std::function<void(Request&, Response&)>& lambda);
    void Delete(const std::string& route, const std::function<void(Request&, Response&)>& lambda);
    void Put(const std::string& route, const std::function<void(Request&, Response&)>& lambda);
    void Options(const std::string& route, const std::function<void(Request&, Response&)>& lambda);
    void Use(const std::string& path, const std::function<void(Request&, Response&)>& lambda);
};