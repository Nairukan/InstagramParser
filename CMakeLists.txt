cmake_minimum_required(VERSION 3.5)

project(Wlad_Parse2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

add_executable(Wlad_Parse2 main.cpp parser.cpp parser.h request.cpp request.h)

target_link_libraries(Wlad_Parse2 ${CURL_LIBRARIES})
#install(TARGETS Wlad_Parse2
#    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
