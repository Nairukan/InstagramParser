cmake_minimum_required(VERSION 3.5)

project(InstagramParser LANGUAGES CXX)

add_compile_definitions(DEBUG_FILE)
#add_compile_definitions(DEBUG)

#add_compile_definitions(Posts)
add_compile_definitions(Reels)
#add_compile_definitions(Test_AutoAcc_DEV)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

#find_package(CURL REQUIRED)
find_package(ExelWorkLib CONFIG REQUIRED) # https://github.com/Nairukan/ExelWorkLib/
find_package(NetworkRequestLib CONFIG REQUIRED) # https://github.com/Nairukan/NetworkRequestLib/
find_package(fmt)
find_package(libzip REQUIRED)

include_directories(#${CURL_INCLUDE_DIR}
    PUBLIC
                    ${ExelWorkLib_INCLUDE_DIR}
                    ${NetworkRequestLib_INCLUDE_DIR}
                )

add_executable(InstagramParser
                    main.cpp
                    instagramutils.h
                )

target_link_libraries(InstagramParser
    PUBLIC
                    #CURL::libcurl
                    fmt
		    libzip::zip
                    ::ExelWorkLib
                    NetworkRequestLib::NetworkRequestLib_static
                )

#install(TARGETS Wlad_Parse2
#    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
