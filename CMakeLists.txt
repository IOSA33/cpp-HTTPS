cmake_minimum_required(VERSION 3.10)
project(httpLib VERSION 0.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(app main.cpp
                   src/Server.cpp
                   src/Request/Request.cpp
                   src/Response/Response.cpp)

target_link_libraries(app PRIVATE ws2_32 stdc++exp)
