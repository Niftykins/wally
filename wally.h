typedef ImageRGB24 Image; //quality of life thing

int main(int, char**);
void resizeBilinear(Image*, int, int, Image*);
void resizeNN(Image*, int, int, Image*);
void flip(Image*);