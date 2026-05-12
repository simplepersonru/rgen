cmake_minimum_required(VERSION 3.20)

project(redkit-gen VERSION 1.0 LANGUAGES C CXX)

# Политика CMake касающаяся нормализации путей для install.
# Не поддерживается версией в контейнере. Но кидает неприятные Warning из сборочного контейнера
# Данное выражение позволяет от них избавиться.
if(POLICY CMP0177)
    cmake_policy(SET CMP0177 NEW) # https://cmake.org/cmake/help/v3.31/policy/CMP0177.html
endif()

# Задание директории установки по-умолчанию
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install)

add_subdirectory(src)
add_subdirectory(export)
enable_testing()

