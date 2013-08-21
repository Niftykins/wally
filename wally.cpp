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

	Image* in;
	Image* bilinear;
	Image* mask;
	Image* nnFlip;

	cout << "Reading from file " << argv[1] << endl;
	in = PNGCodecRGB24::readPNG(argv[1]);

	// 11x13 to 35x41
	double minX = 11, minY = 13;
	double maxX = 35, maxY = 41;
	double multiple = 1.1;
	double scaledX, scaledY;
	stringstream ss;
	for(double x = minX, y = minY; x < maxX, y < maxY; x *= multiple, y *= multiple) {
		scaledX = round(x);
		scaledY = round(y);

		//cout << scaledX << " " << scaledY << endl;

		bilinear = new Image(scaledX, scaledY, 3);
		resizeBilinear(in, scaledX, scaledY, bilinear);

		ss.str(string());		
		ss << scaledX << "x" << scaledY << ".png";
		//cout << "Writing to file: " << ss.str() << endl;
		PNGCodecRGB24::writePNG(ss.str().c_str(), *bilinear);

		flip(bilinear);
		ss.str(string());		
		ss << scaledX << "x" << scaledY << "flip.png";
		PNGCodecRGB24::writePNG(ss.str().c_str(), *bilinear);

		mask = new Image(scaledX, scaledY, 3);
		resizeNN(in, scaledX, scaledY, mask);

		ss.str(string());
		ss << scaledX << "x" << scaledY << "NN.png";
		//cout << "Writing to file: " << ss.str() << endl;
		PNGCodecRGB24::writePNG(ss.str().c_str(), *mask);

		flip(mask);
		ss.str(string());
		ss << scaledX << "x" << scaledY << "NNflip.png";
		PNGCodecRGB24::writePNG(ss.str().c_str(), *mask);
	}

	time(&end);
	cout << difftime(end, start) << " seconds\n";

	delete in;
	delete bilinear;
	delete mask;
}

void resizeBilinear(Image* in, int w2, int h2, Image* out) {
	RGB24 a, b, c, d;
	int x, y, index;
	int w = in->width, h = in->height;
	double ratioX = ((double)(w-1))/w2;
	double ratioY = ((double)(h-1))/h2;
	double red, green, blue, diffX, diffY;

	for(int yy = 0; yy < h2; yy++) {
		for(int xx = 0; xx < w2; xx++) {
			x = (int)(ratioX * xx);
			y = (int)(ratioY * yy);
			diffX = (ratioX * xx) - x;
			diffY = (ratioY * yy) - y;

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
		
			out->data[yy][xx].r = (int)red;
			out->data[yy][xx].g = (int)green;
			out->data[yy][xx].b = (int)blue;
		}
	}
}

void resizeNN(Image* in, int w2, int h2, Image* out) {
	double ratioX = in->width/(double)w2;
	double ratioY = in->height/(double)h2;
	int px, py;

	for(int yy = 0; yy < h2; yy++) {
		for(int xx = 0; xx < w2; xx++) {
			px = floor(xx * ratioX);
			py = floor(yy * ratioY);

			RGB24 pixel = in->data[py][px];

			if((int)pixel.r != 255 && (int)pixel.b != 255 && (int)pixel.b != 255)
				pixel.r = pixel.g = pixel.b = 0;

			out->data[yy][xx] = pixel;
		}
	}
}

void flip(Image* in) {
	Image* out = new Image(in->width, in->height, 3);
	int h = in->height, w = in->width;

	for(int yy = 0; yy < h; yy++) {
		for(int xx = 0; xx < w; xx++) {
			out->data[yy][w-1-xx] = in->data[yy][xx]; 
		}
	}

	for(int yy = 0; yy < h; yy++) {
		for(int xx = 0; xx < w; xx++) {
			in->data[yy][xx] = out->data[yy][xx]; 
		}
	}
}
