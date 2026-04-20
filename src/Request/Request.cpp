#include "Request.h"
#include <string>
#include <print>
#include <chrono>
#include <iostream>
#include <string_view>
#include <nlohmann/json.hpp>

void Request::parser(const std::string& req) {
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

        m_body_string.append(req, it + 4, std::string::npos);

    } else {
        std::println("Request::parser, Didn't found any body!");
        return;
    }

    return;
}

void Request::addBody(const std::string& req) {
    if (!req.empty()) {
        m_body_string.append(req);
    } else {
        std::println("Request::addBody, req is empty!");
    }
}

const std::string& Request::getPath(std::string_view buf) {
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

const std::string& Request::getMethod(std::string_view buf) {
    m_method.reserve(12);
    // Longest method is 6 chars so we prevent from checking whole buf
    for (int i { 0 }; i < 12; ++i) {
        if (buf[i] == ' ') break;
        
        m_method += buf[i];
    }

    return m_method;
}

std::string Request::getHeader(const std::string& headerToFind) const {
    auto header_it { m_headers.find(headerToFind) };

    if (header_it != m_headers.end()) {
        return header_it->second;
    }

    return "";
}

void Request::parseBody() {
    if(!m_body_string.empty()) {
        if(nlohmann::json::accept(m_body_string)) {
            m_body_json = nlohmann::json::parse(m_body_string);
        } else {
            m_body_json = nlohmann::json::object();
            // TODO: parse normal string
        }
    }
}

std::string Request::getBody(std::string_view key) const {
    if (!m_body_json.empty()) {
        if (m_body_json.contains(key)) {
            if (m_body_json.is_primitive()) {
                // TODO: do somthing different
                return m_body_json[key].dump();
            } else if (m_body_json.is_structured()) {
                // TODO: Needs to do something different 
                return m_body_json[key].dump();
            }
        }
    }

    if (!m_body.empty()) {
        // TODO: get normal string
    }
    
    return "";
}