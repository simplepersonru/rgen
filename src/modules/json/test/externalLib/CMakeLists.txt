set(LIB_NAME extLib)

add_library(${LIB_NAME} STATIC extLib.cpp extLib_global.h)

target_include_directories(${LIB_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

find_package(Qt5 COMPONENTS Core)
target_link_libraries(${LIB_NAME} PRIVATE Qt5::Core)
