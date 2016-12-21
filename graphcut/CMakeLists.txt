cmake_minimum_required(VERSION 2.8)
 
PROJECT(Graphcut2D)
 
find_package( OpenCV REQUIRED )

add_executable( Graphcut2D 
 Graphcut2D.cpp
 graphcut.cpp
 stdafx.cpp
 bit_vector.h
 graphcut.h
 graphcutAlgorithm.h
 stdafx.h
 )
include_directories( ${INCLUDE})
target_link_libraries( Graphcut2D ${OpenCV_LIBS} )