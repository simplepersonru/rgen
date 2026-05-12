cmake_minimum_required(VERSION 3.15)

if(POLICY CMP0177)
    cmake_policy(SET CMP0177 NEW) # https://cmake.org/cmake/help/v3.31/policy/CMP0177.html
endif()

set(PROJECT_NAME simple-example)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build_cmake)
project(${PROJECT_NAME} VERSION 1.0 LANGUAGES C CXX)

set(RGEN_DIR "${CMAKE_SOURCE_DIR}/../../../install") 

include("${RGEN_DIR}/cmake/rgen.cmake")

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME}
    main.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE
        ${RGEN_DIR}/include
        )
add_subdirectory(mylib)

target_sources(${PROJECT_NAME} PRIVATE $<TARGET_OBJECTS:mylib>)
rgen_process(${PROJECT_NAME})

