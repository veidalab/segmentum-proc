/*
Description:
Contour3D.cpp reads a raw file from the given filepath and creates a 3D mask which will capture the shape of the object.
Note: If the raw file "Name.raw" has a separate header "Name.mha", the mha header will be read first.
The output 3D mask is saved in a file ending in *.3dm (3d mask) and it contains uchar data with the same dimensions as the original data in the raw file.   
The output file data starts and ends with a separator ‘..m..’. The separator is also used to separate 2D image slices (the depth or z-axis) in the volume data.
A zero (0) indicates background (no object) and a one (1) indicates foreground (object). Each value is separated by a space. 

Calling the process from command line: ./Contour3D filepathMHA filepathRAW
If we do not have an mha file then filepathMHA should be NULL
Output:   "Mask.3dm"
*/


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <ctype.h>
#include <iostream>
#include <fstream>
#include <time.h>

#include <C:/Projects/opencv/build/include/opencv2/core/mat.hpp>
#include <C:/Projects/opencv/build/include/opencv2/core/matx.hpp>
#include <C:/Projects/opencv/build/include/opencv2/imgproc/imgproc.hpp>
#include <C:/Projects/opencv/build/include/opencv2/highgui/highgui.hpp>
#include <cv.h>
#include <ctype.h>
#include <highgui.h>

using std::cout;
using std::ofstream;
using std::ifstream;
using namespace cv;

int main(char *filepath, char *filepathRAW)
{
	//...Variables defined...//
	int dims; double* spacing; int value;
	int idx = 0, x, y, z, framesize;
	ifstream inFile; ofstream outFile;
	char * file; char* datafile; char* maskfile;
	std::string info; std::string eq;  int infoInt;
	unsigned char * imagedata;
	//clock_t t1, t2, t3;

	//...Retrieve information on the data dimensions, type, and size...//
	
	//file = filepath; //NULL if we do not have an mha
	//datafile = filepathRAW;
	
	file = "Foot.mha";
	datafile = "Foot.raw";

	inFile.open(file);
	if (inFile.is_open()) {
		std::cout << "Retrieving data\n";
	}
	else {
		//std::cout << "Error opening file\n";
		return 1;
	}
	FILE * f = fopen(datafile, "rb");
	if (!f)
	{
		//printf("Path Error : %s\n", datafile);
		return 1;
	}

	if (file != NULL)
	{
		inFile >> info >> eq >> dims;
		if (info.find("NDims") != std::string::npos)
		{
			std::cout << "Data dimensions:" << dims << "\n";
		}
		else
		{
			//std::cout << "ERROR RETRIEVING DIMENSIONS!\n";
			return 1;
		}
		inFile >> info >> eq >> x >> y >> z;
		if (info.find("DimSize") != std::string::npos)
		{
			std::cout << "Data size:" << "\t" << x << "\t" << y << "\t" << z << "\n";
		}
		else
		{
			//std::cout << "ERROR RETRIEVING SIZE!\n";
			return 1;
		}
	}
	else
	{
		fseek(f, 0, SEEK_END);
		int size = ftell(f);
		//Assumming x = y = z (same size dimensions)
		double result = cbrt(double(size));
		x = int(result);
		y = x; z = x;
	}
	inFile.close();

	//...Read Raw data and clone to Matrix...//
	framesize = x*y*z;
	int size[3] = {x,y,z};
	
	imagedata = (unsigned char*)malloc(sizeof(unsigned char) * framesize);
	fread(imagedata, sizeof(unsigned char),framesize, f);
	cv::Mat volume(3, size, CV_8UC(1), Scalar::all(0));
	
	for (int i = 0; i<size[0]; i++) {
		for (int j = 0; j<size[1]; j++) {
			for (int k = 0; k<size[2]; k++)
			{
					volume.at<unsigned char>(i, j, k) = imagedata[idx];
				//	cout << int(volume.at<unsigned char>(i, j, k)) << " ";
					idx++;
			}
		}
	}
	//cout << "\nNumber of pixels:" << idx << "\n";
	free(imagedata);
	fclose(f);

	//...Extract a plane from the matrix; Otsu Threshold the slice; Create 3D mask...//
	cv::Mat slice(size[0], size[1], CV_8UC(1), Scalar::all(0)); //create slice matrix
	cv::Mat smooth(size[0], size[1], CV_8UC(1), Scalar::all(0)); // create gaussian filtered matrix
	cv::Mat img_bw(size[0], size[1], CV_8UC(1), Scalar::all(0)); //create binary slice output
	cv::Mat mask(3, size, CV_8UC(1), Scalar::all(0)); //create final binary volume

	maskfile = "Mask.3dm";
	outFile.open(maskfile);
	outFile << "..m.." << " ";

	for (int i = 0; i<size[0]; i++) {
		for (int j = 0; j<size[1]; j++) {
			for (int k = 0; k<size[2]; k++)
		{
			
				slice.at<unsigned char>(j,k) = volume.at<unsigned char>(i, j, k); //fill in original intensity values of slice
				//cout << int(slice.at<unsigned char>(i, j)) << " ";
				
			}
		}
		//namedWindow("imgs", 0);
		//imshow("imgs", slice);
		//waitKey(0);

		//...Otsu's Thresholding...//
		//GaussianBlur(slice, smooth, Size(5,5), 0, 0);
		cv::threshold(slice, img_bw, 0,255, CV_THRESH_TOZERO| CV_THRESH_OTSU);
		//namedWindow("Otsu", 0);
		//imshow("Otsu", img_bw);
		//waitKey(0);

		//...Write to file...//
		for (int j = 0; j<size[1]; j++) {
			for (int k = 0; k<size[2]; k++)
			{
				outFile << int(img_bw.at<unsigned char>(j, k)) << " ";
			}
		}
		outFile << "..m..";
		slice = Scalar::all(0); smooth = Scalar::all(0); img_bw = Scalar::all(0);

	}

	slice.release(); smooth.release(); img_bw.release();

	outFile.close();

	mask.release();
	std::cout << "\n3D Mask filename:" << maskfile << "\n";
	system("pause");

  return EXIT_SUCCESS;
}
