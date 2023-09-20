cmake_minimum_required(VERSION 3.5)

project(InstagramParser LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(InstagramParser main.cpp
)


install(TARGETS InstagramParser
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
