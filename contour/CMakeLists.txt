cmake_minimum_required(VERSION 2.8)
 
PROJECT(Contour3D)
 
find_package( OpenCV REQUIRED )
add_executable( Contour3D Contour3D.cpp )
include_directories( ${INCLUDE})
target_link_libraries( Contour3D ${OpenCV_LIBS} )