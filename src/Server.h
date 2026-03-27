#pragma once

#include <string>
#include <functional>
#include <map>
#include <print>
#include <utility>
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
    // Method, route, origPath, lambda
    std::map<std::string, std::map<std::string, std::pair<std::string, std::function<void(Request&, Response&)>>>> m_routes;
    
public:
    Server(const std::string& ip, int port) 
        : m_port(port), m_ip(ip) {
            std::println("Server Started at Port: {}", port);
        }
    int run();

    // Methods
    void Get(const std::string& route, const std::function<void(Request&, Response&)>& lambda);
    void Post(const std::string& route, const std::function<void(Request&, Response&)>& lambda);
    void Delete(const std::string& route, const std::function<void(Request&, Response&)>& lambda);
    void Put(const std::string& route, const std::function<void(Request&, Response&)>& lambda);
    
    void Use(const std::string& path, const std::function<void(Request&, Response&)>& lambda);
};