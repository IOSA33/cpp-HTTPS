#include <string>
#include <map>
#include <functional>
#include <iostream>
#include <fstream>
#include <print>
#include <format>
#include <algorithm>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include "Response.h"
#include "../Request/Request.h"

void Response::findRouteAndExecute(
        const std::string& method,
        const std::string& path, 
        const std::map<std::string, std::map<std::string, std::pair<std::string, std::function<void(Request&, Response&)>>>>& routes,
        std::string& responseToClient, 
        Request& request,
        Response& response
    ) {

    auto method_it { routes.find(method) };

    if (method_it != routes.end()) {
        auto path_it { method_it->second.find(path) };
        if (path_it != method_it->second.end()) {
            path_it->second.second(request, response);
            responseToClient = response.returnResponse();
            return;        
        }
        std::println("No such Route!");
    }

    std::println("Response::findRouteAndExecute, Didn't found any Route, using USE Middleware!");

    auto middleware_it { routes.find("USE") };
    if (middleware_it != routes.end()) {
        auto pagenotfound { middleware_it->second.find("PageNotFound") };
        if (pagenotfound != middleware_it->second.end()) {
            pagenotfound->second.second(request, response);
            responseToClient = response.returnResponse();
            return;
        }
        std::println("Response::findRouteAndExecute, No such USE Method!");
    }

    pageNotFound();
    responseToClient = response.returnResponse();
}

void Response::sendFile(const std::string& filePath) {
    std::string content_type {};
 
    size_t lastDot = filePath.find_last_of('.');
 
    if (lastDot == std::string::npos) {
        std::cerr << "Response::sendFile, path is not valid!" << '\n';
        pageNotFound();
        return;
    }

    std::string file_extension { filePath.substr(lastDot + 1) };

    if (file_extension == "html"){
        content_type = "text/html";
    } else if (file_extension == "json") {
        content_type = "application/json; charset=utf-8";
    }

    std::string filestring{};
    readFile(filestring, filePath);    

    // C++20
    setHeader("Content-Type", content_type);
    m_response.append(std::format("Content-Length: {}\r\n", filestring.size()));
    m_response.append("\r\n");

    m_response.append(filestring);
}

void Response::readFile(std::string& file, const std::string& filePath) {
    std::ifstream myFile { filePath };

    if(!myFile.is_open()) {
        std::println("Response::readFile, Cannot open a file");
        return;
    }

    std::string line{};

    while(std::getline(myFile, line)) {
        file += line;
    }

    myFile.close();
}

void Response::redirect(std::string_view url) {
    m_response.append(std::format("Location: {} \r\n", url));
    m_response.append("\r\n");
}

void Response::setStatus(int code) {

    m_response.clear();

    if (!(code < 100) && !(code >= 600)) {
        switch (code) {
        case 100:
            m_response.append(std::format("HTTP/1.1 {} Continue\r\n", code)); break;
        case 101:
            m_response.append(std::format("HTTP/1.1 {} Switching Protocols\r\n", code)); break;
        case 200:
            m_response.append(std::format("HTTP/1.1 {} OK\r\n", code)); break;
        case 201:
            m_response.append(std::format("HTTP/1.1 {} Created\r\n", code)); break;
        case 204:
            m_response.append(std::format("HTTP/1.1 {} No Content\r\n", code)); break;
        case 301:
            m_response.append(std::format("HTTP/1.1 {} Moved Permanently\r\n", code)); break;
        case 302:
            m_response.append(std::format("HTTP/1.1 {} Found\r\n", code)); break;
        case 304:
            m_response.append(std::format("HTTP/1.1 {} Not Modified\r\n", code)); break;
        case 400:
            m_response.append(std::format("HTTP/1.1 {} Bad Request\r\n", code)); break;
        case 401:
            m_response.append(std::format("HTTP/1.1 {} Unauthorized\r\n", code)); break;
        case 403:
            m_response.append(std::format("HTTP/1.1 {} Forbidden\r\n", code)); break;
        case 404:
            m_response.append(std::format("HTTP/1.1 {} Not Found\r\n", code)); break;
        case 500:
            m_response.append(std::format("HTTP/1.1 {} Internal Server Error\r\n", code)); break;
        case 502:
            m_response.append(std::format("HTTP/1.1 {} Bad Gateway\r\n", code)); break;
        case 503:
            m_response.append(std::format("HTTP/1.1 {} Service Unavailable\r\n", code)); break;

        default:
            std::println("setStatus(), Default break!"); break;
        }
    } else {
       std::println("Invalid http code: {}!", code); 
    }
}

void Response::setHeader(std::string_view key, std::string_view value) {
    if (!key.empty() || !value.empty()) {
        m_response.append(std::format("{}: {}\r\n", key, value));
    } else {
        std::println("Error in Response::setHeader, Key or Value are empty!");
    }
}

void Response::pageNotFound() {
    const std::string pageNotFoundReply { "404 Not Found" };
    setStatus(404);
    setHeader("Content-Type", "text/html");
    setHeader("Content-Length", std::to_string(pageNotFoundReply.size()));
    setBody(pageNotFoundReply);
}

void Response::json(std::string_view content) {
    if(content.empty()) {
        std::println("Error in Response::json: Content is Empty!");
        return;
    }
    if (nlohmann::json::accept(content)) {
        setHeader("Content-Type", "application/json; charset=utf-8");
        setHeader("Content-Length", std::to_string(content.size()));
        m_response.append("\r\n");
        m_response.append(content);
    } else {
        std::println("Error in Response::json: Json is not valid: {}\n", content);   
    }
}

void Response::setBody(std::string_view content){
    m_response.append("\r\n");
    m_response.append(content);
}

void Response::end() {
    setHeader("Content-Length", "0");
    setHeader("Connection", "close");
    m_response.append("\r\n");
}