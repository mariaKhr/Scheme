cmake_minimum_required(VERSION 3.13)

project(scheme)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)
add_subdirectory(src)

add_executable(${PROJECT_NAME} repl/main.cpp)
target_link_libraries (${PROJECT_NAME} scheme_tidy)