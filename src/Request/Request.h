#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <map>
#include <print>
#include <nlohmann/json.hpp>

class Request {
private:
    std::string m_method{};
    std::string m_path{};
    std::map<std::string, std::string> m_headers{};
    std::map<std::string, std::string> m_body{};
    nlohmann::json m_body_json{ nullptr };
    std::string m_body_string{};

public:
    Request() = default;

    void parser(const std::string& req);
    void addBody(const std::string& req);
    void parseBody();
    void printBodyString() const { std::println("{}", m_body_string); }
    std::string getBody(std::string_view key) const;
    int getReceivedDataSize() const { return static_cast<int>(m_body_string.size()); }
    const std::string& getMethod(std::string_view buf);
    const std::string& getPath(std::string_view buf);
    std::string getHeader(const std::string& headerToFind) const;
};
