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

#include "graphcutAlgorithm.h"

segmentation * seg1;

int main(char* filename, int* point, int seedlen, int** seeds, char focusaxis)
{

	/*char* file = filename;
	char* focusAx = focusaxis;
	int sLen = seedlen; //size of seeds in each array
	int** seed = seeds;
	int* pt = point;*/

	char* fil = "Foot.mha";
	char focusAx = 'z';
	int pt[3] = { 25,68,60 };
	//std::cout << focusAx << "\n";
	int sLen = 20; //size of seeds in each array
	int bs[1433*2];
	int rs[684*2]; int idx; int count = 0;
	ifstream b; ifstream r;
	b.open("C:\\Poomy\\Projects\\2DGC\\bin\\bseeds_idx.txt");
	r.open("C:\\Poomy\\Projects\\2DGC\\bin\\rseeds_idx.txt");
	while(b>>idx)
	{
		int i = idx / 256;//row
		int j = idx % 256;//column
		bs[count] = i; bs[count + 1] = j;
		count = count + 2;
	}
	count = 0;
	while (r >> idx)
	{
		int i = idx / 256;//row
		int j = idx % 256;//column
		
		rs[count] = i; rs[count + 1] = j;
		count = count + 2;

	}
	b.close(); r.close();
	int**seed = new int*[2];
	seed[0] = new int[1433 * 2];
	seed[1] = new int[684 * 2];

	for(int i=0;i<684 * 2;i++)
	{
		seed[1][i] = rs[i];
		//std::cout << seed[1][i] << "\t" <<  "\n";
	}
	for (int i = 0; i<1433 * 2; i++)
	{
		seed[0][i] = bs[i];
		//std::cout << seed[0][i] << "\t" << "\n";
	}

	seg1 = new segmentation(fil, pt, sLen, seed, focusAx);
	seg1->segment();

	delete[] seed, bs, rs, pt;

	return EXIT_SUCCESS;
}
