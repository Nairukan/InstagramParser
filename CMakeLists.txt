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


find_package(fmt REQUIRED)
find_package(libzip REQUIRED)

include_directories(#${CURL_INCLUDE_DIR}
    PUBLIC
                    ${ExelWorkLib_INCLUDE_DIR}
                    ${NetworkRequestLib_INCLUDE_DIR}
                )

#message(WARNING "ExelWorkLib_FOUND = ${ExelWorkLib_FOUND}")

message(WARNING "NetworkRequestLib_INCLUDE_DIR = ${NetworkRequestLib_INCLUDE_DIR}")
message(WARNING "ExelWorkLib_INCLUDE_DIR = ${ExelWorkLib_INCLUDE_DIR}")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++") 


add_executable(InstagramParser 
                    main.cpp
                    instagramutils.h
                )

target_link_libraries(InstagramParser
    PUBLIC

                    #CURL::libcurl_static
                    #fmt::fmt
		            #libzip::zip
                    ExelWorkLib::ExelWorkLib_shared
                    NetworkRequestLib::NetworkRequestLib_shared

                )
#[[
get_cmake_property(_variableNames VARIABLES)
list (SORT _variableNames)
foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()
#]] 
if(WIN32)
#[[
    if(NOT EXISTS "${CMAKE_BINARY_DIR}/cacert.pem")
        # Скачиваем файл
        file(DOWNLOAD
            "https://curl.se/ca/cacert.pem"
            "${CMAKE_BINARY_DIR}/cacert.pem"
        )
    endif()
#]]
    #target_compile_definitions(InstagramParser PRIVATE "CACERT_PATH=\"${CMAKE_BINARY_DIR}/cacert.pem\"")
    #[[
    set (From_cmake_dir_to_library_bin "/../../../bin")

    # Задаем список директорий, откуда брать .dll файлы
    
    set(DLL_DIRS
        "${libzip_DIR}${From_cmake_dir_to_library_bin}"
        "${CURL_DIR}${From_cmake_dir_to_library_bin}"
        "${ExelWorkLib_DIR}/../bin"
        "${NetworkRequestLib_DIR}/../bin"
        "${ZLIB_INCLUDE_DIR}/../bin"
        "${OPENSSL_INCLUDE_DIR}/../bin"

        # Добавь сюда другие директории, если необходимо
    )
    #]]

    # Переменная для хранения всех .dll файлов
    set(ALL_DLL_FILES "")
    list(APPEND ALL_DLL_FILES ${NetworkRequestLib_DLL} ${ExelWorkLib_DLL})

    # Проходим по каждой директории из списка
    #[[
    foreach(DLL_DIR ${DLL_DIRS})
        # Получаем список всех .dll файлов в текущей директории
        file(GLOB DLL_FILES "${DLL_DIR}/*.dll")

        # Добавляем найденные .dll файлы в общую переменную
        list(APPEND ALL_DLL_FILES ${DLL_FILES})
    endforeach()
    #]]

    # Цикл по каждому найденному .dll файлу
    foreach(DLL_FILE ${ALL_DLL_FILES})
        # Извлекаем имя файла без пути
        get_filename_component(DLL_NAME "${DLL_FILE}" NAME)

        # Создаем символическую ссылку в директории целевого исполняемого файла
        add_custom_command(TARGET InstagramParser POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E create_symlink
            "${DLL_FILE}" "$<TARGET_FILE_DIR:InstagramParser>/${DLL_NAME}"
        )
    endforeach()
endif()
