#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <stdexcept>
#include <iostream>

#include <time.h>

#include "PNGCodecRGB24.h"

#define BLACK 0
#define WHITE 255

using namespace std;

typedef struct Stuffs {
	string n;
	int x;
	int y;
	int w;
	int h;
} Stuffs;

Stuffs* make(string n, int x, int y, int w, int h) {
	Stuffs* r = new Stuffs;
	r->n = n;
	r->x = x;
	r->y = y;
	r->w = w;
	r->h = h;

	return r;
}

void check(Stuffs* i) {
	cout << "Checking -> " << i->n << " ... ";
	ImageRGB24* src = PNGCodecRGB24::readPNG(("results/"+i->n+".png").c_str());

	if( (int)src->data[i->y][i->x].r == 255 &&
		(int)src->data[i->y][i->x+i->w-1].r == 255 &&
		(int)src->data[i->y+i->h-1][i->x].r == 255 &&
		(int)src->data[i->y+i->h-1][i->x+i->w-1].r == 255) {
			cout << " passed\n";
	} else cout << " failed\n";
}

int main(int argc, char* argv[]) {
	int NUMBER = 20;
	Stuffs* i[NUMBER];
	
	i[0] = make("AlienCafe", 603, 210, 12, 14);
	i[1] = make("Arena", 90, 249, 12, 14);
	i[2] = make("ArmedCamp", 92, 268, 12, 14);
	i[3] = make("Ballroom", 231, 456, 11, 13);
	i[4] = make("Battle", 334, 237, 12, 14);
	i[5] = make("Beach", 501, 263, 11, 13);
	i[6] = make("Carousels", 105, 143, 12, 14);
	i[7] = make("CrowdedMall", 325, 199, 12, 14);
	i[8] = make("Egypt", 681, 19, 12, 14);
	i[9] = make("Fairground", 598, 98, 11, 13);
	i[10] = make("Expo", 710, 174, 13, 16);
	i[11] = make("Gaul", 87, 345, 11, 13);
	i[12] = make("Goldrush", 522, 150, 11, 13);
	i[13] = make("Market", 171, 142, 15, 17);
	i[14] = make("Maze", 1369, 638, 18, 21);
	i[15] = make("Medieval", 137, 457, 12, 14);
	i[16] = make("Santas", 461, 238, 12, 14);
	i[17] = make("Skiing", 163, 500, 13, 16);
	i[18] = make("Tents", 219, 327, 11, 13);
	i[19] = make("Wembley", 2093, 1344, 35, 41);

	if(argc == 2) {
		for(int ii=0; ii<NUMBER; ii++) {
			if(i[ii]->n.compare(argv[1]) == 0) {
				check(i[ii]);
			}
		}
	} else {
		for(int ii=0; ii<NUMBER; ii++) {
			check(i[ii]);
		}
	}
}












