cmake_minimum_required(VERSION 3.10)

# 1. Name your project
project(MangaBotProcessor)

# 2. Force C++20 (Required for <filesystem>)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 3. Find the CURL library
find_package(CURL REQUIRED)

# 4. Define your executable
# Using ${CMAKE_CURRENT_SOURCE_DIR} is safer than ".." for CLion
add_executable(MANGA_TEST main.cpp)

# 5. Include current directory (where json.hpp lives)
target_include_directories(MANGA_TEST PRIVATE cmake-build-debug)

# 6. Link the Libraries
# We only link CURL.
# We REMOVED the stdc++fs block because GCC 15 handles it automatically.
target_link_libraries(MANGA_TEST PRIVATE CURL::libcurl)