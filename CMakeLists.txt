project( segmentum-proc )
cmake_minimum_required( VERSION 2.8 )

FIND_PACKAGE( OpenCV REQUIRED core highgui imgproc)

add_subdirectory( contour )
add_subdirectory( graphcut )
