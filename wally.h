typedef ImageRGB24 Image; //quality of life thing

typedef struct Result {
	int x;
	int y;
	int scaledX;
	int scaledY;
	int difference;
} Result;

int main(int, char**);
void resizeBilinear(Image*, int, int, Image*);
void resizeNN(Image*, int, int, Image*);
void flip(Image*);
Result* search(Image*, Image*, Image*);
void box(Image*, int, int, int, int);
