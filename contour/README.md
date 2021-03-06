----------------------
I. File list
----------------------
Contour3D.cpp               Main file for the object's shape and boundaries
CMakeLists.txt              The CMake file for configuration
README.txt                  This file
----------------------
Command Line Arguments
----------------------
Configure using CMake:        cd projectDirectory/source
                              cmake ..
               
Calling the function:         ./Contour3D filename.mha
Parameter:                    filename.mha  -> The 3D image file
Example:                      ./Contour3D  Foot.mha
Output:                        mask.3dm
----------------------
II. Description
----------------------
Contour3D.cpp takes a ".mha" filename as parameter and reads a 3D image file to extract the shape of the volume (3D image). The ".mha" 
file is a header file containing information  on the raw data. The program fetches the contained information (data dimensions, size, 
data type, byte order, and the filename of the raw data), then it opens the raw data file. The raw data is cloned to a cv::Mat object 
(opencv 3D matrix). Afterwards, 2D matrices are extracted from the 3D matrix to perform Otsu's Thresholding, where the resulting mask 
contains values '0' indicating no object, and '1' indicating the object. Once the thresholding is done, the 2D matrices are stacked back 
to a 3D matrix, and the result is written to a file with  *.3dm (3d mask) ending. The file contains 1D arrays (with values '0' and '1' only), 
where each array represents an image slice. Each array is separated by "..m.." and each value is separated by a space " ". The separator 
"..m.." is also used at the beginning and end of the file.
----------------------
III. Future changes
----------------------
This project currently uses CMake and CLion/Visual Studio for configuration and building.
make, automake, and autoconf will be used instead of cmake.