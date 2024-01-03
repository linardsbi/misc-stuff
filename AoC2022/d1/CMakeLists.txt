cmake_minimum_required(VERSION 3.23)
project(d1)
add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
set(CMAKE_CXX_STANDARD 23)

add_executable(d1
        d1.cpp)
