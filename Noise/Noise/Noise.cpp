// Noise.cpp: определяет точку входа для консольного приложения.
//


#include "stdafx.h"
#include <opencv2\opencv.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

IplImage* image = 0;
IplImage* dst = 0;


float noise(int x, int y)
{
	int n = x + y * 57;
	n = (n << 13) ^ n;
	return (1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

float smoothNoise(float x, float y) {
	float corners = (noise(x - 1, y - 1) + noise(x + 1, y - 1) + noise(x - 1, y + 1) + noise(x + 1, y + 1)) / 16;
	float sides = (noise(x - 1, y) + noise(x + 1, y) + noise(x, y - 1) + noise(x, y + 1)) / 8;
	float center = noise(x, y) / 4;
	return (corners + sides + center);
}


float cosine_interpolate(float a, float b, float x) {
	float ft = x * 3.1415927;
	float f = (1 - cos(ft)) * .5;
	return  (a*(1 - f) + b*f);
}


float interpolatedNoise(float x, float y) {
	int x1 = (int)x;
	float x2 = x - x1;
	int y1 = (int)y;
	float y2 = y - y1;
	float v1 = smoothNoise(x1, y1);
	float v2 = smoothNoise(x1 + 1, y1);
	float v3 = smoothNoise(x1, y1 + 1);
	float v4 = smoothNoise(x1 + 1, y1 + 1);

	float i1 = cosine_interpolate(v1, v2, x2);
	float i2 = cosine_interpolate(v3, v4, x2);

	return (cosine_interpolate(i1, i2, y2));


}

float PerlinNoise2d(int x, int y) {
	float total = 0;
	float pres = (float)0.01;
	int n = 3;
	float freq;
	float ampl = CV_PI;
	for (int i = 0; i < n; i++)
	{
		freq = 2 * i;
		total = total + interpolatedNoise(x * freq, y * freq) * ampl;
	}return total;

}




int main(int argc, char* argv[])
{

	char* filename = "D:\\casia\\CASIA-Iris-Interval\\247\\R\\S1247R01.jpg";
	char* filename1 = "D:\\eyes1\\Noise_new_Gabor\\S1247R01_99.61%.jpg";

	image = cvLoadImage(filename, 1);
	assert(image != 0);

	dst = cvCloneImage(image);

	int count = 0;

	cvNamedWindow("original", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("noise", CV_WINDOW_AUTOSIZE);

	float min = (float)10000000;
	float max = (float)-10000.8;

	for (int y = 0; y<dst->height; y++) {
		uchar* ptr = (uchar*)(dst->imageData + y * dst->widthStep);
		for (int x = 0; x<dst->width; x++) {

			if ((((unsigned int)(PerlinNoise2d(x, y) * 1000000)) % 10000) >= 9961) {

				ptr[3 * x] = ((unsigned int)(PerlinNoise2d(x, y) * 1000000)) % 255;
				ptr[3 * x + 1] = ((unsigned int)(PerlinNoise2d(x, y) * 1000000)) % 255;
				ptr[3 * x + 2] = ((unsigned int)(PerlinNoise2d(x, y) * 1000000)) % 255;

				count++;

			}
		}
	}

	std::cout << filename;
	printf("[i] noise: %d (%.4f%c)\n", count, (float)count / (dst->height*dst->width) * 100, '%');
	std::cout << "max= " << max << " " << "min= " << min;

	cvSaveImage(filename1, dst);

	cvShowImage("original", image);
	cvShowImage("noise", dst);
	cvWaitKey(0);


	cvReleaseImage(&image);
	cvReleaseImage(&dst);


	cvDestroyWindow("original");
	cvDestroyWindow("noise");
	return 0;
}