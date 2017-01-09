----------------------
I. File list
----------------------
Graphcut2D.cpp              Main file that calls the segmentation function
graphcut.cpp                This file allocates memory and initializes the graph representation of the image
stdafx.cpp                  This file contains the standard includes 
stdafx.h                    Header file with standard system include files
graphcut.h                  Header file to calculate min-cut/max-flow on the graph from its respective cpp file
graphcutAlgorithm.h         Header file to read the image and seed files and perform segmentation
bit_vector.h                Header file for bit vector
CMakeLists.txt              The CMake file for configuration
README.txt                  This file 
----------------------
Command Line Arguments
----------------------
Configure using CMake:        cd projectDirectory/source
                              cmake ..
               
Calling the function:         ./Graphcut2D filename.mha [x,y,z] seedlength [seedsAdr] focusaxis
Current Parameters:           filename.mha  -> The 3D image file
                              int* point    -> x, y, z point of interest for segmentation
                              int seedlength   -> size of the 2 seed arrays
                              int** seeds   -> double pointer pointing to 2 1D arrays (1 array for object seeds and 1 for background seeds)
                              char focusaxis-> the axis/image slice orientation of interest for segmentation
Example:                      ./Graphcut2D  Foot.mha [51,23,60] 600 [*seedO,*seedB] z
Outputs:                       intensity.2di    -> intensity values of segmented image
                               polyShape.2ds    -> polydata shape/contour of segmented image
----------------------
II. Description
----------------------
Graphcut2D.cpp takes the parameters mentioned above, allocates and initializes all the given information. The raw data is cloned to a
cv::Mat object (opencv 3D matrix) and a 2D matrix is extracted using the x,y,z point and focus axis information. The 2D image data is then 
segmented using the graphcut algorithm and the provided seeds. After this process, the image is filtered using the resulting graphflow to 
obtain the object of interest and ignore the unwanted pixels. The resulting image is saved to a *.2di file. The file contains uchar information
of the pixel values of the segmented data in 1D arrays, where each array represents an image slice. Each array is separated by "..i.." and
each value is separated by a space " ". The separator "..i.." is also used at the beginning and end of the file. The same image slice obtained 
after segmentation is filtered using a contour technique, where the resulting mask contains polydata represented as triangles. This result is
written as stl file with  *.2ds ending. 
----------------------
III. Future changes
----------------------
This project currently uses CMake and CLion/Visual Studio for configuration and building.
make, automake, and autoconf will be used instead of cmake to build an *.so file.
How parameters are passed to the function may change.
