#include <print>
#include "src/Server.h"
#include "src/Request/Request.h"
#include "src/Response/Response.h"

int main() {
    Server server{ "0.0.0.0" , 6788 };

    server.Use("PageNotFound", [](Request& req, Response& res) -> void {
        res.setStatus(301);
        res.redirect("https://www.youtube.com");
    });

    auto middleware {
        [](const std::function<void(Request&, Response&)>& next) {
            return [next](Request& req, Response& res) {
    
                // Here we doing some handling
                // Then we call our main func if our check for something is success
    
                // For example
                if (req.getHeader("Host") == "localhost:6788") {
                    next(req, res);
                } else {
                    std::println("The connection Host is not a localhost:6788!");
                    res.setStatus(302);
                    res.redirect("localhost:6788/jsontest");
                }
            };
        }
    };

    server.Get("/htmltest", middleware([](Request& req, Response& res) -> void {
        res.setStatus(200);
        res.setHeader("User-Agent", "App/1.0");
        std::println("Host is: {}", req.getHeader("Host"));
        
        res.sendFile("../html/test.html");
    }));

    server.Post("/htmltest", [](Request& req, Response& res) -> void {
        res.setStatus(201);
        std::println("Content-Length is: {}", req.getHeader("Content-Length"));
        res.sendFile("../html/htmlPostTest.html");
    });
    
    server.Get("/jsontest", [](Request& req, Response& res) -> void {
        res.setStatus(200);
        res.sendFile("../html/test.json");
    });

    int err { server.run() };
    if (err == 1) {
        std::println("Error to Start The Server!");
    }

    return 0;
}