#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <stdexcept>
#include <iostream>

#include <time.h>

#include "PNGCodecRGB24.h"
#include "wally.h"

#define BLACK 0
#define WHITE 255

using namespace std;

Image* in;
Image* src;
int frog =0;

void* fancy(void* kek) {
	Fancy* f = (Fancy*)kek;
	Image* base = f->base;
	Image* mask = f->mask;
	int ex=f->ex, ey=f->ey, sx=f->sx, sy=f->sy;
	int lowest = 1337433434;
	Result* result = new Result;

	int area = base->width * base->height;

	//cout << "f: " << ex-mask->width << " s: " << sx << endl;
	//cout << "fs: " << ey-mask->height << " s: " << sy << endl;

	Image* maskFlip = new Image(mask->width, mask->height, 3);
	flip(mask, maskFlip);
	Image* baseFlip = new Image(base->width, base->height, 3);
	flip(base, baseFlip);

	for(int yy=sy; yy < ey - mask->height; yy++) {
		for(int xx=sx; xx < ex - mask->width; xx++) {
			int sum = 0, sumFlip = 0;
			for(int maskY = 0; maskY < mask->height; maskY++) {
				for(int maskX = 0; maskX < mask->width; maskX++) { //for each pixel in the mask
					RGB24 sp = src->data[yy+maskY][xx+maskX];
					RGB24 mp = mask->data[maskY][maskX];
					RGB24 mfp = maskFlip->data[maskY][maskX];

					if(mp.r == BLACK) { //only need to test one as they're all either 0 or 255
						RGB24 bp = base->data[maskY][maskX];
						sum += abs(sp.r-bp.r) + abs(sp.g-bp.g) + abs(sp.b-bp.b);
					}
					if(mfp.r == BLACK) {
						RGB24 bfp = baseFlip->data[maskY][maskX];
						sumFlip += abs(sp.r-bfp.r) + abs(sp.g-bfp.g) + abs(sp.b-bfp.b);
					}
				}
				
			}

			if(lowest > sum) {
				lowest = sum;
				result->x = xx;
				result->y = yy;
				result->difference = sum/area;
			}
			if(lowest > sumFlip) {
				lowest = sumFlip;
				result->x = xx;
				result->y = yy;
				result->difference = sumFlip/area;
			}
		}
	}

	delete maskFlip;
	delete baseFlip;

	f->result = result;
}

void* loop(void* info) {
	Stuff* r = (Stuff*)info;
	int scaledX = r->x, scaledY = r->y;
	pthread_t t[3];
	Fancy* f[3];
	int lowest = 1337433434;

	Image* bilinear = new Image(scaledX, scaledY, 3);
	resizeBilinear(in, scaledX, scaledY, bilinear);
	Image* mask = new Image(scaledX, scaledY, 3);
	createMask(in, scaledX, scaledY, mask);
	
	//r->result = search(src, mask, bilinear);

	f[0] = new Fancy;
	f[0]->base = bilinear;
	f[0]->mask = mask;
	f[0]->sx = 0;
	f[0]->sy = 0;
	f[0]->ex = src->width/2;
	f[0]->ey = src->height/2;

	f[1] = new Fancy;
	f[1]->base = bilinear;
	f[1]->mask = mask;
	f[1]->sx = src->width/2+1;
	f[1]->sy = 0;
	f[1]->ex = src->width;
	f[1]->ey = src->height;

	f[2] = new Fancy;
	f[2]->base = bilinear;
	f[2]->mask = mask;
	f[2]->sx = src->width/2 - mask->width/2-1;
	f[2]->sy = 0;
	f[2]->ex = src->width/2 + mask->width/2+1;
	f[2]->ey = src->height;

	pthread_create(&t[0], NULL, fancy, (void*)f[0]);
	pthread_create(&t[1], NULL, fancy, (void*)f[1]);
	pthread_create(&t[2], NULL, fancy, (void*)f[2]);
	pthread_join(t[0], NULL);
	pthread_join(t[1], NULL);
	pthread_join(t[2], NULL);

	for(int ii = 0; ii < 3; ii++) {
		//cout << f[ii]->result->difference << endl;
		if (lowest > f[ii]->result->difference) {
			lowest = f[ii]->result->difference;
			r->result = f[ii]->result;
		}
	}
	//r->result = f->result;
	r->result->w = scaledX;
	r->result->h = scaledY;

	cout << scaledX << "x" << scaledY << " -> x: " << r->result->x << " y: " << r->result->y << " " << r->result->difference << " " << endl;

	delete bilinear;
	delete mask;
}

int main(int argc, char *argv[]) {
	if(argc != 4) {
		cout << "\nUsage:\n";
		cout << argv[0] << " base_template.png search_image.png result.png \n\n";
		exit(0);
	} 

	time_t end, start;
	time(&start);

	pthread_t t[20];
	Stuff* info[20];

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
	for(double x = minX, y = minY; x < maxX && y < maxY; x *= multiple, y *= multiple, ii++) {
		scaledX = round(x);
		scaledY = round(y);

		info[ii] = new Stuff;
		info[ii]->x = scaledX;
		info[ii]->y = scaledY;
		
		pthread_create(&t[ii], NULL, loop, (void*)info[ii]);
	}

	for(int jj=0; jj<ii; jj++) {
		pthread_join(t[jj], NULL);

		if(lowest->difference > info[jj]->result->difference) {
			lowest = info[jj]->result;
		}
	}

	cout << "\nBest match: \nx: " << lowest->x << "\ny: " << lowest->y << "\ndifference: " << lowest->difference << endl << endl;

	box(src, lowest->x, lowest->y, lowest->w, lowest->h);
	PNGCodecRGB24::writePNG(argv[3], *src);
	cout << "Boxed image written to " << argv[3] << endl;

	time(&end);
	cout << difftime(end, start) << " seconds\n";
	cout << "frog: " <<frog << endl;

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
						sum += abs(sp.r-bp.r + sp.g-bp.g + sp.b-bp.b);
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
	for(int yy = y; yy < y+h; yy++) {
		for(int xx = x; xx < x+w; xx++) {
			if((xx == x || xx == x+w-1) || (yy == y || yy == y+h-1)) {
				src->data[yy][xx].r = WHITE;
				src->data[yy][xx].g = BLACK;
				src->data[yy][xx].b = BLACK;
			}
		}
	}
}
