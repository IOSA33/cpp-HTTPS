#include "Request.h"
#include <string>
#include <vector>
#include <algorithm>
#include <print>
#include <chrono>
#include <iostream>
#include <string_view>

void Request::parser(const std::string& req) {
    auto start { std::chrono::steady_clock::now() };
    auto it = req.find("\r\n\r\n");

    if (it != std::string::npos) {
        std::string_view headers = req.substr(req.find("\r\n") + 2, it);
        
        std::cout << headers << '\n';

        bool valueBool { false };
        std::string key {};
        key.reserve(32);
        std::string value {};
        value.reserve(32);
        
        for(size_t i { 0 }; i < headers.size(); ++i) {
            if (headers[i] == '\r') {
                i += 1;
                valueBool = false;
                m_headers.emplace(key, value);
                key.clear();
                value.clear();
                continue;
            }

            if (headers[i] == ':' &&  headers[i + 1] == ' ') {
                i += 1;
                valueBool = true;
                continue;
            }

            if (valueBool == false) {
                key += headers[i];                
            }
 
            if (valueBool == true) {
                value += headers[i];
            }
        }

        auto end { std::chrono::steady_clock::now() };
        auto duration {std::chrono::duration<double, std::milli>(end - start)};
        std::println("Time used: {}", duration);

        //m_body.append(req.substr(it + 4));
    } else {
        std::println("Request::parser, Didn't found any body!");
        return;
    }

    return;
}

void Request::addBody(const std::string& req) {
    if (!req.empty()) {
        auto it = req.find("=");
        m_body.insert({req.substr(0, it), req.substr(it + 1)});
    } else {
        std::println("Request::addBody, req is empty!");
    }
}

// will use it for example url/body/page/{1}/
// about/example  :  {"abour", "example"}
void Request::splitURL(const std::string& url) {
    std::string token{};

    for (const auto& i: url) {
        if (i == '/') {
            m_vec_path.push_back(token);
            token.clear();
        } else {
            token.push_back(i);
        }
    }
}

std::string Request::getPath(const std::string_view buf) {
    std::string path{};
    path.reserve(32);
    // We know that http path is always second, so we do thr check for whitespacec
    short whitespaceAppear{ 0 };
    for (const auto& i : buf) {
        if (i == ' ') {
            ++whitespaceAppear;
            if (whitespaceAppear > 1) break;
            continue;
        }

        if (whitespaceAppear == 1) {
            path += i;
        }
    }

    return path;
}

const std::string& Request::getMethod(const std::string_view buf) {
    m_method.clear();
    m_method.reserve(6);
    // Longest method is 6 chars so we prevent from checking whole buf
    for (int i { 0 }; i < 5; ++i) {
        if (buf[i] == ' ') break;
        
        m_method += buf[i];
    }

    return m_method;
}

std::string Request::getHeader(const std::string& headerToFind) const {
    auto header_it { m_headers.find(headerToFind) };

    if (header_it != m_headers.end()) {
        return header_it->second;

    } else {
        std::println("Request::getHeader, Didn't found such Header: {}", headerToFind);
    }

    return "";
}