#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdexcept>
#include <iostream>
#include <sstream>

#include <time.h>

#include "PNGCodecRGB24.h"
#include "wally.h"

using namespace std;

int main(int argc, char *argv[]) {
	if(argc != 4) {
		cout << "\nUsage:\n";
		cout << argv[0] << " base_template.png search_image.png result.png \n\n";
		//exit(0);
	} 

	time_t end, start;

	time(&start);

	ImageRGB24* in;
	ImageRGB24* bilinear;
	string file = "head.png";

	cout << "Reading from file " << file << endl;
	in = PNGCodecRGB24::readPNG(file.c_str()); //convert string to const char*

	cout << file << endl << in->width << " " << in->height << " " << in->depth << endl;

	// 11x13 to 35x41
	double minX = 11, minY = 13;
	double maxX = 35, maxY = 41;
	double multiple = 1.1;
	double scaledX, scaledY;
	for(double x = minX, y = minY; x < maxX, y < maxY; x *= multiple, y *= multiple) {
		scaledX = round(x);
		scaledY = round(y);

		//cout << scaledX << " " << scaledY << endl;

		bilinear = new ImageRGB24(scaledX, scaledY, in->depth);

		resizeBilinear(in, scaledX, scaledY, bilinear);

		stringstream ss;
		ss << scaledX << "x" << scaledY << ".png";
		cout << "Writing to file: " << ss.str() << endl;
		PNGCodecRGB24::writePNG(ss.str().c_str(), *bilinear);
	}

	time(&end);
	cout << difftime(end, start) << " seconds\n";

	delete in;
	delete bilinear;
}

void resizeBilinear(ImageRGB24* in, int w2, int h2, ImageRGB24* out) {
	RGB24 a, b, c, d;
	int x, y, index;
	int w = in->width, h = in->height;
	float ratioX = ((float)(w-1))/w2;
	float ratioY = ((float)(h-1))/h2;
	float red, green, blue, diffX, diffY;

	for(int ii = 0; ii < h2; ii++) {
		for(int jj= 0; jj < w2; jj++) {
			x = (int)(ratioX * jj);
			y = (int)(ratioY * ii);
			diffX = (ratioX * jj) - x;
			diffY = (ratioY * ii) - y;

			a = in->data[y][x];
			b = in->data[y][x+1];
			c = in->data[y+1][x];
			d = in->data[y+1][x+1];

            blue = (a.b)*(1-diffX)*(1-diffY) + (b.b)*(diffX)*(1-diffY) + 
            	(c.b)*(diffY)*(1-diffX)   + (d.b)*(diffX*diffY);

            green = ((a.g))*(1-diffX)*(1-diffY) + ((b.g))*(diffX)*(1-diffY) + 
            	((c.g))*(diffY)*(1-diffX)   + ((d.g))*(diffX*diffY);

            red = ((a.r))*(1-diffX)*(1-diffY) + ((b.r))*(diffX)*(1-diffY) + 
            	((c.r))*(diffY)*(1-diffX)   + ((d.r))*(diffX*diffY);
		
			out->data[ii][jj].r = (int)red;
			out->data[ii][jj].g = (int)green;
			out->data[ii][jj].b = (int)blue;
		}
	}
}

/*
for each scale - 10% increments
	flip = false
	search

	flip = true
	search
end for

*/

/* horizonal flip
for (int row = 0; row < input.rows; ++row) {
 for (int col = 0; col < input.cols; ++col) {
  output.pixels[row][input.cols-1-col].red = input.pixels[row][col].red;
  output.pixels[row][input.cols-1-col].green = input.pixels[row][col].green;
  output.pixels[row][input.cols-1-col].blue = input.pixels[row][col].blue;
 }
}
*/