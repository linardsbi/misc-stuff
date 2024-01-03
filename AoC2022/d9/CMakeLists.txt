cmake_minimum_required(VERSION 3.23)
project(main)
add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -O0 -g -fsanitize=address)
add_link_options(-fsanitize=address)
set(CMAKE_CXX_STANDARD 23)

add_executable(main d9.cpp)