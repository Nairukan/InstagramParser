cmake_minimum_required(VERSION 3.5)

project(InstagramParser LANGUAGES CXX)

#add_compile_definitions(DEBUG)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

add_executable(InstagramParser main.cpp request.cpp request.h instagramutils.h
)

target_link_libraries(InstagramParser ${CURL_LIBRARIES})

#install(TARGETS Wlad_Parse2
#    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
