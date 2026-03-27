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
    std::map<std::string, std::string> m_body{};
    int m_received_data_size{};

public:
    Request() = default;

    void parser(const std::string& req);
    void addBody(const std::string& req);
    const std::map<std::string, std::string>& getBody() const { return m_body; }
    const int getReceivedDataSize() const { return m_received_data_size; }
    void splitURL(const std::string& url);
    const std::string& getMethod(const std::string_view buf);
    std::string getPath(const std::string_view buf);
    std::string getHeader(const std::string& headerToFind) const;
};
