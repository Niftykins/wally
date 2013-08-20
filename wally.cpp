#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdexcept>
#include <iostream>
#include <sstream>

#include "PNGCodecRGB24.h"
#include "wally.h"

using namespace std;

int main(int argc, char *argv[]) {
	if(argc != 4) {
		cout << "\nUsage:\n";
		cout << argv[0] << " base_template.png search_image.png result.png \n\n";
		//exit(0);
	} 

	ImageRGB24* in;
	ImageRGB24* out;
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

		out = new ImageRGB24(in->width, in->height, in->depth);

		//resize(in, scaledX, scaledY, out);

		stringstream ss;
		ss << scaledX << "x" << scaledY << ".png";
		cout << "Writing to file: " << ss.str() << endl;
		PNGCodecRGB24::writePNG(ss.str().c_str(), *out);
	}

	delete in;
	delete out;
}

void resize(ImageRGB24* in, int width, int height, ImageRGB24* out) {

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