cmake_minimum_required(VERSION 3.5)

project(InstagramParser LANGUAGES CXX)

#add_compile_definitions(DEBUG_FILE)
add_compile_definitions(DEBUG)

#add_compile_definitions(Posts)
add_compile_definitions(Reels)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)
find_package(ExelWorkLib CONFIG REQUIRED) # https://github.com/Nairukan/ExelWorkLib/
find_package(NetworkRequestLib CONFIG REQUIRED) # https://github.com/Nairukan/NetworkRequestLib/

include_directories(${CURL_INCLUDE_DIR}
                    ${ExelWorkLib_INCLUDE_DIR}
                    ${NetworkRequestLib_INCLUDE_DIR}
                )

add_executable(InstagramParser
                    main.cpp
                    instagramutils.h
                )

target_link_libraries(InstagramParser
                    ${CURL_LIBRARIES}
                    ::ExelWorkLib
                    ::NetworkRequestLib
                )

#install(TARGETS Wlad_Parse2
#    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
