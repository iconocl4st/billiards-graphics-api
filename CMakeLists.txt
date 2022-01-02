cmake_minimum_required(VERSION 3.13)
project(graphics_api)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_VERBOSE_MAKEFILE on)

find_package(Threads REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
#find_package (Eigen3 3.3 REQUIRED NO_MODULE HINTS /usr/share/cmake/Modules/ /usr/share/eigen3/cmake/)

add_executable(
    graphics_api

	${CMAKE_CURRENT_SOURCE_DIR}/src/graphics_api.cpp
)

target_include_directories(
	graphics_api
	
	PUBLIC

	${PROJECT_SOURCE_DIR}/src

	${PROJECT_SOURCE_DIR}/../billiards-common/src
	$ENV{REPOS}/billiards-common/src
)


target_link_libraries(
    graphics_api

	Threads::Threads
	${Boost_SYSTEM_LIBRARY}
	Eigen3::Eigen
)

install(TARGETS graphics_api DESTINATION /app)
