include_directories(${LLVM_INCLUDE_DIRS})
enable_testing()

add_google_test(comment_test comment_test.cpp)
add_google_test(file_test file_test.cpp)
add_google_test(string_util_test string_util_test.cpp)
add_google_test(registry_test registry_test.cpp)
add_google_test(preprocessor_test preprocessor_test.cpp)

# Подключать либу std::filesystem нужно только для Linux
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(file_test PRIVATE stdc++fs)
endif()
