set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/gcc")

project( segmentum-proc )
cmake_minimum_required( VERSION 2.8 )

add_subdirectory( contour )
add_subdirectory( graphcut )
