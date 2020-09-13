cmake_minimum_required(VERSION 3.14)

project(mess)

set(CMAKE_CXX_STANDARD 20)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

find_package(OpenSSL REQUIRED)

set(ENABLE_PROGRAMS OFF CACHE BOOL "" FORCE)
set(ENABLE_TESTING OFF CACHE BOOL "" FORCE)

add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(modules/mbedtls)
add_subdirectory(modules/spdlog)

target_include_directories(server PRIVATE "${PROJECT_SOURCE_DIR}/modules" ${OPENSSL_INCLUDE_DIR})
target_link_libraries(server PRIVATE spdlog ${OPENSSL_LIBRARIES})

target_include_directories(client PRIVATE "${PROJECT_SOURCE_DIR}/modules")
target_link_libraries(client PRIVATE mbedtls spdlog)