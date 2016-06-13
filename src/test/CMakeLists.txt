include_directories(${liblinal_SOURCE_DIR}/include)
add_executable(test_app src/tests.cpp)
target_link_libraries(test_app gtest gtest_main)