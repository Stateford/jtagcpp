cmake_minimum_required(VERSION 3.19)

project(jtagcpp)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_library(${PROJECT_NAME} INTERFACE)


add_subdirectory(tests)
