#include <stdio.h>
#include <time.h>
#include <complex>
#include <math.h>
#include <stdlib.h>

int main(int argc, char* argv) {
	time_t end, start, end1, start1, end2, start2;
	time(&start);

	char x = -20;
	for(long int ii=0; ii<1000000000; ii++) {
		char sum =  (x^((~((x>>7)&1))+1)) + ((x>>7)&1);
	}
	time(&end);

	time(&start1);
	for(long int ii=0; ii<1000000000; ii++) {
		int sum = abs(x);
	}
	time(&end1);

	time(&start2);
	for(long int ii=0; ii<1000000000; ii++) {
		int sum = std::abs(x);
	}
	time(&end2);

	printf("%f\n", difftime(end, start));
	printf("%f\n", difftime(end1, start1));
	printf("%f\n", difftime(end2, start2));
}