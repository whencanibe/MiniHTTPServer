cmake_minimum_required(VERSION 3.30)
project(MiniHttpServer)

set(CMAKE_CXX_STANDARD 20)

add_executable(MiniHttpServer src/main.cpp
        src/server.cpp
        include/server.h
        src/session.cpp
        include/session.h
        src/router.cpp
        include/router.h
        src/util.cpp
        include/util.h
        src/db.cpp
        include/db.h
        include/thread_pool.h)

add_custom_command(TARGET MiniHttpServer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/static
        ${CMAKE_BINARY_DIR}/static)


# nlohmann/json via FetchContent (git)
include(FetchContent)
FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG        v3.12.0
)
FetchContent_MakeAvailable(json)
target_link_libraries(MiniHttpServer PRIVATE nlohmann_json::nlohmann_json)

find_package(SQLite3 REQUIRED)
target_link_libraries(MiniHttpServer PRIVATE SQLite::SQLite3)
