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
        std::string headers = req.substr(req.find("\r\n") + 2, it);

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

        const auto body_it { req.substr(it + 4) };
        m_received_data_size = body_it.size();

        if (m_received_data_size > 0) {
            const auto it { body_it.find("=") };
            m_body.insert({body_it.substr(0, it), body_it.substr(it + 1)});
        }

    } else {
        std::println("Request::parser, Didn't found any body!");
        return;
    }

    return;
}

void Request::addBody(const std::string& req) {
    if (!req.empty()) {
        const auto it { req.find("=") };
        m_body.insert({req.substr(0, it), req.substr(it + 1)});
    } else {
        std::println("Request::addBody, req is empty!");
    }

    for(const auto& i: m_body) {
        std::cout << i.first << " = " << i.second << '\n';
    }
}

const std::string& Request::getPath(const std::string_view buf) {
    m_path.reserve(16);
    // We know that http path is always second, so we do thr check for whitespacec
    short whitespaceAppear{ 0 };
    for (const auto& i : buf) {
        if (i == ' ') {
            ++whitespaceAppear;
            if (whitespaceAppear > 1) break;
            continue;
        }

        if (whitespaceAppear == 1) {
            m_path += i;
        }
    }

    return m_path;
}

const std::string& Request::getMethod(const std::string_view buf) {
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