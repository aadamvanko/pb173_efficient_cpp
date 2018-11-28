cmake_minimum_required(VERSION 2.8)

project(ex10)
LINK_LIBRARIES(pthread)
add_executable(${PROJECT_NAME} "main.cpp")
