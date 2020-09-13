cmake_minimum_required(VERSION 3.14)

project(server)

set(source_dir "${PROJECT_SOURCE_DIR}/src")

file(GLOB_RECURSE src "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE hdr "${PROJECT_SOURCE_DIR}/src/*.h")

add_executable(
	server
	${src}
	${hdr}
)