project(liblinal VERSION 0.1 LANGUAGES CXX)

add_library(linal include/linal/vec.hpp)
set_target_properties(linal PROPERTIES LINKER_LANGUAGE CXX)

include_directories(include)