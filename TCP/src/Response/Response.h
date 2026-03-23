#pragma once

#include <string_view>
#include <map>
#include <functional>
#include <string>
#include <string_view>

class Request;

class Response { 
private:
    std::string m_response{};

public:
    Response() = default;

    void findRouteAndExecute(const std::string& method, const std::string& path, const std::map<std::string, std::map<std::string, std::pair<std::string, std::function<void(Request&, Response&)>>>>& routes, std::string& responseToClient, Request& request, Response& response);
    void sendFile(const std::string& filePath);
    void readFile(std::string& file, const std::string& filePath);
    void redirect(std::string_view url);
    void setStatus(int code);
    std::string_view returnResponse() { return m_response; };
    void setHeader(std::string_view key, std::string_view value);
    void pageNotFound();
    void json(std::string_view content);
    void setBody(std::string_view content);
    void end();
};