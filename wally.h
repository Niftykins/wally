typedef ImageRGB24 Image; //quality of life thing

typedef struct Result {
	int x;
	int y;
	int w;
	int h;
	int difference;
} Result;

typedef struct Stuff {
	int x;
	int y;
	Result* result;
} Stuff;

typedef struct Fancy {
	int sy;
	int sx;
	int ey;
	int ex;
	Image* mask;
	Image* base;
	Result* result;
} Fancy;

int main(int, char**);
void resizeBilinear(Image*, int, int, Image*);
void createMask(Image*, int, int, Image*);
void flip(Image*, Image*);
Result* search(Image*, Image*, Image*);
void box(Image*, int, int, int, int);
void* fancy(void*);