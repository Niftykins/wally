#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <stdexcept>
#include <iostream>
#include <complex>

#include <time.h>

#include "PNGCodecRGB24.h"
#include "wally.h"

#define BLACK 0
#define WHITE 255

using namespace std;

Image* in;
Image* src;

void* loop(void* info) {
	Stuff* r = (Stuff*)info;
	int scaledX = r->x, scaledY = r->y;

	
	r->result = search(src, r->mask, r->base);
	
	r->result->w = scaledX;
	r->result->h = scaledY;

	cout << scaledX << "x" << scaledY << " -> x: " << r->result->x << " y: " << r->result->y << " " << r->result->difference << " " << endl;

	return NULL;
}

int main(int argc, char *argv[]) {
	if(argc != 4) {
		cout << "\nUsage:\n";
		cout << argv[0] << " base_template.png search_image.png result.png \n\n";
		exit(0);
	} 

	pthread_t t[26];
	Stuff* info[26];

	cout << "Reading from base image " << argv[1] << endl;
	in = PNGCodecRGB24::readPNG(argv[1]);

	cout << "Reading from source image " << argv[2] <<endl;
	src = PNGCodecRGB24::readPNG(argv[2]);

	Result* lowest = new Result;
	lowest->difference = 9038490;

	// 11x13 to 35x41 // 
	double minX = 11, minY = 13;
	double maxX = 35, maxY = 41;
	double multiple = 1.1;
	int scaledX, scaledY, ii=0;

	time_t end, start;
	time(&start);

	for(double x = minX, y = minY; x < maxX && y < maxY; x *= multiple, y *= multiple, ii+=2) {
		scaledX = round(x);
		scaledY = round(y);

		Image* bilinear = new Image(scaledX, scaledY, 3);
		resizeBilinear(in, scaledX, scaledY, bilinear);
		Image* mask = new Image(scaledX, scaledY, 3);
		createMask(in, scaledX, scaledY, mask);

		info[ii] = new Stuff;
		info[ii]->x = scaledX;
		info[ii]->y = scaledY;
		info[ii]->base = bilinear;
		info[ii]->mask = mask;
		
		pthread_create(&t[ii], NULL, loop, (void*)info[ii]);

		Image* maskFlip = new Image(mask->width, mask->height, 3);
		flip(mask, maskFlip);
		Image* baseFlip = new Image(bilinear->width, bilinear->height, 3);
		flip(bilinear, baseFlip);

		info[ii+1] = new Stuff;
		info[ii+1]->x = scaledX;
		info[ii+1]->y = scaledY;
		info[ii+1]->base = baseFlip;
		info[ii+1]->mask = maskFlip;
		
		pthread_create(&t[ii+1], NULL, loop, (void*)info[ii+1]);
	}

	for(int jj=0; jj<ii; jj++) {
		pthread_join(t[jj], NULL);

		if(lowest->difference > info[jj]->result->difference) {
			lowest = info[jj]->result;
		}
	}

	time(&end);

	cout << "\nBest match: \nx: " << lowest->x << "\ny: " << lowest->y << "\ndifference: " << lowest->difference << endl << endl;

	box(src, lowest->x, lowest->y, lowest->w, lowest->h);
	PNGCodecRGB24::writePNG(argv[3], *src);
	cout << "Boxed image written to " << argv[3] << endl;

	cout << difftime(end, start) << " seconds\n";

	delete in;
	delete src;
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

//nn resize
void createMask(Image* in, int w2, int h2, Image* out) {
	double ratioX = in->width/(double)w2;
	double ratioY = in->height/(double)h2;
	int px, py, sum;

	for(int yy = 0; yy < h2; yy++) {
		for(int xx = 0; xx < w2; xx++) {
			px = floor(xx * ratioX);
			py = floor(yy * ratioY);

			RGB24 pixel = in->data[py][px];

			if((int)pixel.r != WHITE && (int)pixel.b != WHITE && (int)pixel.b != WHITE) {
				pixel.r = pixel.g = pixel.b = BLACK;
			}

			out->data[yy][xx] = pixel;
		}
	}
}

void flip(Image* in, Image* out) {
	int h = in->height, w = in->width;

	for(int yy = 0; yy < h; yy++) {
		for(int xx = 0; xx < w; xx++) {
			out->data[yy][w-1-xx] = in->data[yy][xx]; 
		}
	}
}

Result* search(Image* src, Image* mask, Image* base) {
	int lowest = 1337433434;
	Result* result = new Result;
	result->x = -1;
	result->y = -1;

	int area = base->width * base->height;

	for(int yy = 0; yy < src->height - mask->height; yy++) {
		for(int xx = 0; xx < src->width - mask->width; xx++) { //for each pixel in the src image
			int sum = 0, sumFlip = 0;
			for(int maskY = 0; maskY < mask->height; maskY++) {
				for(int maskX = 0; maskX < mask->width; maskX++) { //for each pixel in the mask
					RGB24 sp = src->data[yy+maskY][xx+maskX];
					RGB24 mp = mask->data[maskY][maskX];

					if(mp.r == BLACK) { //only need to test one as they're all either 0 or 255
						RGB24 bp = base->data[maskY][maskX];
						sum += std::abs(sp.r-bp.r) + std::abs(sp.g-bp.g) + std::abs(sp.b-bp.b);
						//sum += ((sp.r-bp.r)^((~(((sp.r-bp.r)>>31)&1))+1)) + (((sp.r-bp.r)>>31)&1) + ((sp.g-bp.g)^((~(((sp.g-bp.g)>>31)&1))+1)) + (((sp.g-bp.g)>>31)&1) + ((sp.b-bp.b)^((~(((sp.b-bp.b)>>31)&1))+1)) + (((sp.b-bp.b)>>31)&1);

					}
				}
				
			}

			if(lowest > sum) {
				lowest = sum;
				result->x = xx;
				result->y = yy;
				result->difference = sum/area;
			}
		}
	}

	return result;
}

void box(Image* src, int x, int y, int w, int h) {
	for(int yy = y; yy < y+h+1; yy++) {
		for(int xx = x; xx < x+w+1; xx++) {
			if((xx == x || xx == x+w) || (yy == y || yy == y+h)) {
				src->data[yy][xx].r = WHITE;
				src->data[yy][xx].g = BLACK;
				src->data[yy][xx].b = BLACK;
			}
		}
	}
}
