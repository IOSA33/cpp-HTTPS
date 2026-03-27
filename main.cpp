#include <print>
#include "src/Server.h"
#include "src/Request/Request.h"
#include "src/Response/Response.h"

int main() {
    Server server{ "0.0.0.0" , 6788 };

    // OpenSSl

    // PageNotFound is an emun for Use middleware (check "server.h" for more)
    // server.Use("PageNotFound", [](Request& req, Response& res) -> void {
    //     res.setStatus(301);
    //     res.redirect("https://www.youtube.com");
    // });

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

    server.Get("/html", middleware([](Request& req, Response& res) -> void {
        res.setStatus(200);
        res.setHeader("User-Agent", "App/1.0");
        std::println("Host is: {}", req.getHeader("Host"));
        
        res.sendFile("../html/test.html");
    }));

    server.Post("/mainpage", [](Request& req, Response& res) -> void {
        res.setStatus(201);
        std::println("Content-Length is: {}", req.getHeader("Content-Length"));
        // Path is coming from the "build" folder
        res.sendFile("../html/htmlPostTest.html");
    });
    
    server.Get("/json", [](Request& req, Response& res) -> void {
        res.setStatus(200);
        std::string json = R"({"password":"123", "test":"testApi"})";
        res.json(json);
    });

    server.Get("/favicon.ico", [](Request& req, Response& res) -> void {
        res.setStatus(404);
        // end() is required function if we dont sent any data to the client
        res.end();
    });

    int err { server.run() };
    if (err == 1) {
        std::println("Error to Start The Server!");
    }

    return 0;
}