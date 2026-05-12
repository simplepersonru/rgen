cmake_minimum_required(VERSION 3.14)
project(rgen_json_qt)

find_package(Qt5 REQUIRED COMPONENTS Core)

add_library(rgen_json_qt STATIC
    ../../src/jsonlib_qt.cpp
    ../../include/rgen/private/jsonlib_qt.h
)

target_include_directories(rgen_json_qt PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include  
)

target_link_libraries(rgen_json_qt PUBLIC Qt5::Core)

set_target_properties(rgen_json_qt PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
