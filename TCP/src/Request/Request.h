#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <map>

class Request {
private:
    std::string m_method{};
    std::vector<std::string> m_vec_path{};
    std::map<std::string, std::string> m_headers{};
    std::map<std::string, std::string> m_body1{};
    std::string m_body{};

public:
    Request() = default;

    void parser(const std::string& req);
    void addBody(const std::string& req, const int bytesRecieved);
    const std::string& getBody() const { return m_body; }
    void splitURL(const std::string& url);
    const std::string& getMethod(const std::string_view buf);
    std::string getPath(const std::string_view buf);
    std::string getHeader(const std::string& headerToFind) const;
};
