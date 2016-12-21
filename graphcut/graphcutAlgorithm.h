#ifndef graphcutAlgorithm
#define graphcutAlgorithm

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <ctype.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <cstdlib>
#include <map>
#include <memory>

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

#include "graphcut.h"

#define K 1000
#define Ki 1
//#define sigma2 1000
#define const1 100
#define const2 1000
#define distance 50

/* A simple bit-vector. Everyone's written their own-- here's mine.
   It's endian neutral so life is much easier to deal with on multiple
   platforms.  It probably would be more efficient to do the
   operations on words, but I don't want to get into the endian
   issue. Allows you to set / clear and query a value. All one needs
   for sets.

   Walter Bell (wbell@cs.cornell.edu) 
*/
class segmentation 
{
public:

	int imgheight, imgwidth;
	int npixels;
	int i, j,idx, intensity,counter=0;
	int new_idx;
	ifstream inFile;
	ofstream outFile; ofstream infor;
	int max_thresh, min_thresh;
	int max_threshred, min_threshred;
	double value;
	double int_p, int_q;
	CvScalar img_data;
	int height, width;
	int row, column;
	bool error_p;
	//double sigma;
	bool * redArray;
	int redArrIdx;
	bool redFlag;
	int * inImgData;
	char focusAxis; int** seed; int sLen; int* bseed; int* rseed;
	int* pt;
	int dims; double* spacing;
	int x, y, z, framesize;
	char * file; char* datafile; char* maskfile;
	std::string info; std::string eq;  int infoInt;
	unsigned char * imagedata; 
	//clock_t t1, t2, t3;

	
  segmentation(char* filename, int* point, int seedlen, int** seeds, char focusaxis)
  {
	 file = filename; 
	 focusAxis = focusaxis; //"x", "y", or "z"
	 std::cout << focusAxis << "\n";
	 max_thresh = min_thresh = 0;
	 max_threshred = min_threshred = 0;
	 value = 0;
	 redArrIdx = 0;

	 pt = point;
 
	 sLen = seedlen; //size of seeds in each array
	 seed = (int **)malloc(sizeof(int *) * 2);
	 seed = seeds;
	 bseed = new int[1433 * 2];
	 rseed = new int[684 * 2];
	 for (int i = 0; i<684 * 2; i++)
	 {
		 rseed[i]= seed[1][i];
		 //std::cout << seed[1][i] << "\t" <<  "\n";
		 //system("pause");
	 }
	 for (int i = 0; i<1433 * 2; i++)
	 {
		 bseed[i]= seed[0][i];
		 //std::cout << seed[0][i] << "\t" << "\n";
		// system("pause");
	 }

  }

  int segment()
  {
	  outFile.open("C:\\results\\final.txt");
	  infor.open("C:\\results\\info.txt");
	  //...Read file and extract slice of interest...//
	  //file = "Foot.mha";  //
	  datafile = "Foot.raw";   //
	  inFile.open(file);
	  if (inFile.is_open()) {
		  std::cout << "Retrieving data\n";
	  }
	  else {
		  //std::cout << "Error opening file\n";
		  return 1;
	  }
	  //if (str.find('.mha') != string::npos)
	  //{
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
	  inFile.close();

	  //...Read Raw data and clone to Matrix...//
	  framesize = x*y*z;
	  int size[3] = { x,y,z };

	  FILE * f = fopen(datafile, "rb");
	  if (!f)
	  {
		  //printf("Path Error : %s\n", datafile);
		  return 1;
	  }

	  imagedata = (unsigned char*)malloc(sizeof(unsigned char) * framesize);
	  fread(imagedata, sizeof(unsigned char), framesize, f);
	  cv::Mat volume(3, size, CV_8UC(1), Scalar::all(0));
	  idx = 0;
	  for (int i = 0; i<size[0]; i++) {
		  for (int j = 0; j<size[1]; j++) {
			  for (int k = 0; k<size[2]; k++)
			  {
				  volume.at<unsigned char>(i, j, k) = imagedata[idx];
				 // cout << int(volume.at<unsigned char>(i, j, k)) << " ";
				 // cout << int(imagedata[idx]) << " ";
				  idx++;
			  }
		  }
	  }
	  //cout << "\nNumber of pixels:" << idx << "\n";
	  free(imagedata);
	  fclose(f);

	  if (focusAxis == 'x')
	  {
		  cv::Mat slice(size[1], size[2], CV_8UC(1), Scalar::all(0)); //create slice matrix
		  cv::Mat finalimg(size[1], size[2], CV_8UC(1), Scalar::all(0));
		  
		  int i = pt[0];
		  height = size[2];
		  width = size[1];
		  npixels = height* width;
		  inImgData = new int[npixels];

		  for (int k = 0; k < size[2]; k++) {
			  for (int j = 0; j < size[1]; j++)
			  {
				  slice.at<unsigned char>(j, k) = volume.at<unsigned char>(i, j, k); //fill in original intensity values of slice
				  inImgData[counter] = int(slice.at<unsigned char>(j, k));
				  counter++;
				 //cout << int(slice.at<unsigned char>(i, j)) << " ";
			  }
		  }
	  }
	  else if (focusAxis == 'y')
	  {
		  cv::Mat slice(size[0], size[2], CV_8UC(1), Scalar::all(0)); //create slice matrix
		  cv::Mat finalimg(size[0], size[2], CV_8UC(1), Scalar::all(0));
		  
		  int j = pt[1];
		  height = size[2];
		  width = size[0];
		  npixels = height* width;
		  inImgData = new int[npixels];

		  for (int k = 0; k < size[2]; k++) {
			  for (int i = 0; i < size[0]; i++)
			  {
				  slice.at<unsigned char>(i, k) = volume.at<unsigned char>(i, j, k); //fill in original intensity values of slice
				  inImgData[counter] = int(slice.at<unsigned char>(i, k));
				  counter++;
				  //cout << int(slice.at<unsigned char>(i, j)) << " ";
			  }
		  }
	  }
	  else if (focusAxis == 'z')
	  {
		  cv::Mat slice(size[0], size[1], CV_8UC(1), Scalar::all(0)); //create slice matrix
		  cv::Mat finalimg(size[0], size[1], CV_8UC(1), Scalar::all(0));
		
		  int k = pt[2];
		  height = size[1];
		  width = size[0];
		  npixels = height* width;
		  inImgData = new int[npixels];

		  for (int j = 0; j < size[1]; j++) {
			  for (int i = 0; i < size[0]; i++) 
			  {
				  slice.at<unsigned char>(i, j) = volume.at<unsigned char>(i, j, k); //fill in original intensity values of slice
				  inImgData[counter] = int(slice.at<unsigned char>(i, j));
				 // cout << int(slice.at<unsigned char>(i, j)) << " ";
				 // cout << int(inImgData[counter]) << " ";
				  counter++;
			  }
		  }
	  }
	 
	  else
	  {
		  return 1;
	  }

	BKCutGraph* graph = new BKCutGraph(npixels);
	i = 0;
	while (i < 1433 * 2)
	{
		row = bseed[i]; column = bseed[i+1];
		idx = row*width + column; i = i + 2;
		//std::cout << row << "\t" << column <<"\t" <<idx << "\t" <<  "\n";
		//system("pause");
		graph->add_weight(idx,graph->SOURCE,K);  
		//graph->add_weight(idx,graph->SINK,-K); 
		//for the given pixel, try to find intensity, to set up some thresholds
		min_thresh = 255;
		//i = idx / width ;//row
		//j = idx % width;//column
		//img_data = cvGet2D(img,i,j); // get the (i,j) pixel value
		//intensity = inImgData[idx];
		//if (max_thresh < intensity)
			//max_thresh = intensity;
		//if (min_thresh > intensity)
			//min_thresh = intensity;
		/*now min_thresh contains the lowest intensity value of the pixel.
		max_thresh contains the highest intensity value of the pixel*/ 
		
		//info << "Index:" << idx << " => Row:" << i << ", Column:" << j << "\n";
		
		/*outFile << "We are analyzing the following Image Pixel right now:";		
		outFile << "Index:" << idx << " => Row:" << i << ", Column:" << j << ", Intensity:" << intensity ;
		outFile << "\n";*/
    }
	infor << "min:" << min_thresh << " max:" << max_thresh << "\n";

	////info.close();
	//info.open("C:\\results\\info.txt");

	redArray = new bool[npixels];
	for(counter=0;counter<npixels;counter++)
		redArray[counter] = false;
	idx = 0; i = 0;
	while (i < 684 * 2)
	{		
		row = rseed[i]; column = rseed[i + 1];
		idx = row*width + column; i = i + 2; 
		redArray[idx] = true;
		//std::cout << row << "\t" << column << "\t" << idx << "\t" << "\n";
		//system("pause");
		//i = idx / width ;//row
		//j = idx % width;//column
		//img_data = cvGet2D(img,i,j); // get the (i,j) pixel value
		//intensity = inImgData[idx];
		min_threshred = 255;
		//graph->add_weight(idx,graph->SOURCE,-K); 
		graph->add_weight(idx,graph->SINK,K); 
		if (max_threshred < intensity)
			max_threshred = intensity;
		if (min_threshred > intensity)
			min_threshred = intensity;
    }       
	infor << "RED min:" << min_threshred << " max:" << max_threshred << "\n";
	redArrIdx = 0;

	i = 0;
	redFlag = false;

	while(i<npixels)
	{
		if(((int)((i/width)%2)) == 1)
		{
			//we are looking for only odd numbered rows
			for(j=0;j<width;j++)
			{
				//loop through all columns of this row
				//UP LINK
				if ((i-width) >= 0) //check if this is the first row (not usually possible)
				{
					//row = i/width;
					//idx = (row*width)+j;
					//row indicates the current row and j indicates the current column
					//img_data = cvGet2D(img,row,j); // get the (i,j) pixel value
					//int_p = img_data.val[0];
					int_p= inImgData[i];
					//row = (i-width)/width;
					//idx = (row*width)+j;
					//img_data = cvGet2D(img,row,j); // get the (i,j) pixel value
					int_q = inImgData[i-width];
					//info << "Row:" << row << " Column: " << j ;
					//info << " intensity1:" << int_p;
					//info << " intensity2: " << int_q;
					/*code Naaz 29th Jan 2009*/
					if(!redArray[i])
					{
						
						if(int_p <= max_thresh && int_p >= min_thresh)
						{
							infor << "taken0";
							if(int_p <= max_threshred && int_p >= min_threshred)
							{
								infor << "taken1";
								
								//graph->add_weight(i,graph->SOURCE,1);
								//graph->add_weight(i,graph->SINK,1);
							}
							else
							{
								infor << "taken2";
								//good candidate for foreground
								graph->add_weight(i,graph->SOURCE,Ki);
								//graph->add_weight(i,graph->SINK,-Ki);
							}
						}
						else
						{
							infor << "taken3";
							if(int_p <= max_threshred && int_p >= min_threshred)
							{
								infor << "taken4";
								//graph->add_weight(i,graph->SOURCE,-Ki); 
								graph->add_weight(i,graph->SINK,Ki);
							}
						}
					}
					else
					{
						infor << "taken5";
						graph->add_weight(i,graph->SOURCE,-K); 
						graph->add_weight(i,graph->SINK,K);
					}
					if(!redArray[i-width])
					{
						if(int_q <= max_thresh && int_q >= min_thresh)
						{
							//good candidate for foreground
							graph->add_weight(i-width,graph->SOURCE,Ki);
							//graph->add_weight(i-width,graph->SINK,-Ki);
						}
						else
						{
							//graph->add_weight(i-width,graph->SOURCE,-Ki); 
							graph->add_weight(i-width,graph->SINK,Ki);
						}
					}
					else
					{
						//graph->add_weight(i-width,graph->SOURCE,-K); 
						graph->add_weight(i-width,graph->SINK,K);
					}
				
					
					value = calculate_weight(int_p, int_q);
					graph->add_edge(i, (i-width),value, value);	
					
				}
				
				if (i < (width*(height-1))) //down link
				{
					//row = i / width;
					//idx = (row*width)+j;
					//int_p= inImgData[idx];
					int_p= inImgData[i];
					int_q = inImgData[i+width];
					//row = i / width;
					//img_data = cvGet2D(img,row,j); // get the (i,j) pixel value
					//int_p = img_data.val[0];
					//row = (i+width) / width;
					//img_data = cvGet2D(img,row,j); // get the (i,j) pixel value
					//int_q = img_data.val[0];
					if(!redArray[i])
					{
						if(int_p <= max_thresh && int_p >= min_thresh)
						{
							if(int_p <= max_threshred && int_p >= min_threshred)
							{						
								//good candidate for foreground
								graph->add_weight(i,graph->SOURCE,1);
								graph->add_weight(i,graph->SINK,1);
							}
							else
							{
								graph->add_weight(i,graph->SOURCE,Ki);
								//graph->add_weight(i,graph->SINK,-Ki);
							}
						}
						else
						{
							if(int_p <= max_threshred && int_p >= min_threshred)
							{
								//graph->add_weight(i,graph->SOURCE,-Ki); 
								graph->add_weight(i,graph->SINK,Ki);
							}
						}
					}
					else
					{
						//graph->add_weight(i,graph->SOURCE,-K); 
						graph->add_weight(i,graph->SINK,K);
					}
					if(!redArray[i+width])
					{
						if(int_q <= max_thresh && int_q >= min_thresh)
						{
							//good candidate for foreground
							graph->add_weight(i+width,graph->SOURCE,Ki);
							//graph->add_weight(i+width,graph->SINK,-Ki);
						}
						else
						{
							//graph->add_weight(i+width,graph->SOURCE,-Ki); 
							graph->add_weight(i+width,graph->SINK,Ki);
						}
					}
					else
					{
						//graph->add_weight(i+width,graph->SOURCE,-K); 
						graph->add_weight(i+width,graph->SINK,K);
					}
					//value = -((int_p - int_q)*(int_p - int_q))/sigma2;
					//int_p = exp(value)* sigma;
					//int_q = exp(value)* sigma;
					//graph->add_edge(i, (i+width),int_p, int_q);
					value = calculate_weight(int_p, int_q);
					graph->add_edge(i, (i+width),value, value);				
				}
				i++;
			}
		}
		else
		{
			i = i+width;
		}
	}

	for(i=0; i<npixels; i++)
	{
		//we are constructing only right and left links, alternate nodes only
		
		if(((int)i%2) == 1)
		{		
			if (((int)(i%width)) != 0) //left link
			{
				/*row = i / width;
				column = i % width;
				img_data = cvGet2D(img,row,column); // get the (i,j) pixel value
				int_p = img_data.val[0];
				row = (i-1) / width;
				img_data = cvGet2D(img,row,column); // get the (i,j) pixel value
				int_q = img_data.val[0];*/
				int_p= inImgData[i];
				int_q = inImgData[i-1];
				if(!redArray[i])
				{
					if(int_p <= max_thresh && int_p >= min_thresh)
					{
						if(int_p <= max_threshred && int_p >= min_threshred)
						{						
							//good candidate for foreground
							graph->add_weight(i,graph->SOURCE,1);
							graph->add_weight(i,graph->SINK,1);
						}
						else
						{
							graph->add_weight(i,graph->SOURCE,Ki);
							//graph->add_weight(i,graph->SINK,-Ki);
						}
					}
					else
					{
						if(int_p <= max_threshred && int_p >= min_threshred)
						{
							//graph->add_weight(i,graph->SOURCE,-Ki); 
							graph->add_weight(i,graph->SINK,Ki);
						}
					}
				}
				else
				{
					//graph->add_weight(i,graph->SOURCE,-K); 
					graph->add_weight(i,graph->SINK,K);
				}
				if(!redArray[i-1])
				{
					if(int_q <= max_thresh && int_q >= min_thresh)
					{
						//good candidate for foreground
						graph->add_weight(i-1,graph->SOURCE,Ki);
						//graph->add_weight(i-1,graph->SINK,-Ki);
					}
					else
					{
						//graph->add_weight(i-1,graph->SOURCE,-Ki); 
						graph->add_weight(i-1,graph->SINK,Ki);
					}
				}
				else
				{
					//graph->add_weight(i-1,graph->SOURCE,-K); 
					graph->add_weight(i-1,graph->SINK,K);
				}
				
				value = calculate_weight(int_p, int_q);
				graph->add_edge(i, (i-1),value, value);
			}
			if (((i+1)%width) != 0) //right link
			{
				/*row = i / width;
				column = i % width;
				img_data = cvGet2D(img,row,column); // get the (i,j) pixel value
				int_p = img_data.val[0];
				row = (i+1) / width;
				img_data = cvGet2D(img,row,column); // get the (i,j) pixel value
				int_q = img_data.val[0];*/
				int_p= inImgData[i];
				int_q = inImgData[i+1];
				if(!redArray[i])
				{
					if(int_p <= max_thresh && int_p >= min_thresh)
					{
						if(int_p <= max_threshred && int_p >= min_threshred)
						{						
							//good candidate for foreground
							graph->add_weight(i,graph->SOURCE,1);
							graph->add_weight(i,graph->SINK,1);
						}
						else
						{
							graph->add_weight(i,graph->SOURCE,Ki);
							//graph->add_weight(i,graph->SINK,-Ki);
						}
					}
					else
					{
						if(int_p <= max_threshred && int_p >= min_threshred)
						{
							//graph->add_weight(i,graph->SOURCE,-Ki); 
							graph->add_weight(i,graph->SINK,Ki);
						}
					}
				}
				else
				{
					//graph->add_weight(i,graph->SOURCE,-K); 
					graph->add_weight(i,graph->SINK,K);
				}
				if(!redArray[i+1])
				{
					if(int_q <= max_thresh && int_q >= min_thresh)
					{
						//good candidate for foreground
						graph->add_weight(i+1,graph->SOURCE,Ki);
						//graph->add_weight(i+1,graph->SINK,-Ki);
					}
					else
					{
						//graph->add_weight(i+1,graph->SOURCE,-Ki); 
						graph->add_weight(i+1,graph->SINK,Ki);
					}
				}
				else
				{
					//graph->add_weight(i+1,graph->SOURCE,-K); 
					graph->add_weight(i+1,graph->SINK,K);
				}
					
					value = calculate_weight(int_p, int_q);
					graph->add_edge(i, (i+1),value, value);
			}
		}
	}
	delete [] redArray;
    
	graph->maxflow();

	error_p = graph->error();
	
	cv::Mat finalimg(size[0], size[1], CV_8UC(1), Scalar::all(0));
	for(idx=0;idx<npixels;idx++)
	{
		i = idx/width;
		j = idx%width;
		outFile << graph->what_segment(idx) << "\n";
		if (graph->what_segment(idx) == 1)//SOURCE
		{
			img_data.val[0] = inImgData[idx];//object of interest is displayed in original colors
			finalimg.at<unsigned char>(i, j) = unsigned char(img_data.val[0]); 
			//img_data.val[0]=cvGet2D(img,i,j).val[0];//blue implies object of interest	
			
		}
		else
		{
			
			img_data.val[0] = 0;
			finalimg.at<unsigned char>(i, j) = unsigned char(img_data.val[0]);
			
		}
	}
	namedWindow("seg", 0);
	imshow("seg", finalimg);
	waitKey(0);
	/*finalImgColor = cvCreateImage(cvSize(width,height),IPL_DEPTH_8U, 3);
	for(idx=0;idx<npixels;idx++)
	{
		i = idx/width;
		j = idx%width;
		outFile << graph->what_segment(idx) << "\n";
		if (graph->what_segment(idx) == 1)//SOURCE
		{
			img_data.val[0]=255;//blue implies object of interest
			img_data.val[1] = 0;
			img_data.val[2] = 0;
			cvSet2D(finalImgColor,i,j,img_data);
		}
		else
		{
			img_data.val[2]=255;//red implies background
			img_data.val[0] = 0;
			img_data.val[1] = 0;
			cvSet2D(finalImgColor,i,j,img_data);
		}
	}
	*/
	delete [] inImgData, seed, bseed, rseed;	
	/*cvNamedWindow( "Segmented Image", CV_WINDOW_AUTOSIZE );		
	cvShowImage("Segmented Image", finalImgColor );
	cvWaitKey(0);
	
	//finalimg = cvCreateImage(cvSize(height,width),IPL_DEPTH_8U,1);
	cvSaveImage("C:\\results\\finalimg.jpg",finalimg);
	cvSaveImage("C:\\results\\finalimgcolor.jpg",finalImgColor);
	cvDestroyWindow( "Segmented Image" );	
	cvReleaseImage( &img );
	cvReleaseImage( &finalimg );
	cvReleaseImage( &finalImgColor );*/
	outFile.close();
	infor.close();

	
	

	return 0;
}

  double calculate_weight(int p_int, int q_int)
{
	double power, final_value, temp;	
	temp = p_int - q_int;
	power = -(temp*temp/const1);
	final_value = exp(power)* const2;
	return (final_value);
}

  
};
#endif