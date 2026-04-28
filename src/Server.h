#pragma once

#include <string>
#include <functional>
#include <map>
#include <print>
#include <utility>
#include <winsock2.h>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <vector>
#include <queue>
#include <openssl/ssl.h>
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
    // Method, route, origPath, lambda
    std::map<std::string, std::map<std::string, std::pair<std::string, std::function<void(Request&, Response&)>>>> m_routes;

    // ThreadPool
    bool m_stop_threads{ false };
    std::mutex m_mutex{};
    std::condition_variable m_condition_variable{};
    std::vector<std::thread> m_vector_of_threads{};
    std::queue<std::function<void()>> m_jobs_queue{};
    std::vector<SOCKET> m_vActive_clientSockets{};

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

private:
    // ThreadPool
    void ThreadLoop();
    void StartThreadPool();
    void AddQueueJob(const std::function<void()>& job);
    void StopThreads();
    bool taskInQueue();
    void shotdownAndCloseClientSockets();
};