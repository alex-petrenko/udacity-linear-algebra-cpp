include_directories(${liblinal_SOURCE_DIR}/include)

set(SOURCES
    src/vec_tests.cpp
    src/line_tests.cpp
    src/plane_tests.cpp
    src/util_tests.cpp
)

set(HEADERS
)

add_executable(test_app ${SOURCES} ${HEADERS})
target_link_libraries(test_app gtest gtest_main)