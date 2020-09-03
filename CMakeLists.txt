cmake_minimum_required(VERSION 3.14)

project(mess)

set(CMAKE_CXX_STANDARD 20)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

file(GLOB_RECURSE source_files "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE header_files "${PROJECT_SOURCE_DIR}/src/*.h")

add_subdirectory(modules/spdlog)

add_executable(
	mess
	${source_files}
	${header_files}
)

target_include_directories(mess PRIVATE "${PROJECT_SOURCE_DIR}/modules")
target_link_libraries(mess PRIVATE spdlog)