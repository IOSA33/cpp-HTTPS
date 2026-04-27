cmake_minimum_required(VERSION 3.10)
project(httpLib VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(app main.cpp
                   src/Server.cpp
                   src/Request/Request.cpp
                   src/Response/Response.cpp)

target_link_libraries(app PRIVATE stdc++exp ssl crypto)
set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra -march=native")