add_definitions(-D_SCL_SECURE_NO_WARNINGS)  # VS annoying warnings
add_definitions(-D_USE_MATH_DEFINES)  # to enable stuff like M_PI

add_subdirectory(3rdparty)
add_subdirectory(linal)
add_subdirectory(apps)
add_subdirectory(test)