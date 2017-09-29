cmake_minimum_required(VERSION 2.6)

project(Pathfinder)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS -std=c++11)



set(CMAKE_CONFIGURATION_TYPES Release)
set(CMAKE_BUILD_TYPE Release)

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
set(64BIT 1)
MESSAGE("Building for 64bit")
else()
MESSAGE("Building for 32bit")
endif()

FILE(GLOB_RECURSE HEADERS  "include/*.h*")
FILE(GLOB_RECURSE SOURCES  "src/main.cpp")
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
include_directories(
	${SDL2_INCLUDE_DIR}
	${SDL2_IMAGE_INCLUDE_DIR}
	${INCLUDE_DIR}
	)
	target_link_libraries(
		${PROJECT_NAME} ${SDL2_LIBRARY}
		${SDL2_IMAGE_LIBRARIES}
		${HEADERS}
	)
		