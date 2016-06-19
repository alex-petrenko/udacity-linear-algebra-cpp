project(liblinal VERSION 0.1 LANGUAGES CXX)

set(SOURCES
)

set(HEADERS
    include/linal/vec.hpp
    include/linal/util.hpp
    include/linal/sfinae.hpp
)

add_library(linal ${SOURCES} ${HEADERS})
set_target_properties(linal PROPERTIES LINKER_LANGUAGE CXX)

include_directories(include)