#include <libpng12/png.h> //osx
// #include <png.h> //linux
#include <stdexcept>


#ifndef _H_PNGCODECRGB24
#define _H_PNGCODECRGB24


//
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//
// class ImageRGB24
//
// Purpose:
//  Basic struct for colour RGB-24 images
//
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
//
struct RGB24
{
	// Pixel data is organised BGR in this version
	unsigned char b;
	unsigned char g;
	unsigned char r;
};

class ImageRGB24
{
 //
 ////////////////////////////////////////////////////
 // PUBLIC MEMBERS
 // Everything is public for maximum speed, and forces students to deal with the data directly
 ////////////////////////////////////////////////////
 //
 public:
	int width;				// Width of image
	int height;				// Height of image
	int depth;				// Number of channels (greyscale = 1 channel, RGB = 3)
	RGB24** data;			// Pixel data, indexed by [y][x]

	ImageRGB24(int inWidth, int inHeight, int inDepth);
	~ImageRGB24();
};



//
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//
// class PNGCodecRGB24
//
// Purpose:
//  Helper functions to read and write PNG files
//  These *could* have been put inside Image, but then Image would be
//  intrinsically tied to PNG files, and that's probably not great design
//
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
//
class PNGCodecRGB24 
{
 //
 ////////////////////////////////////////////////////
 // PUBLIC MEMBERS
 ////////////////////////////////////////////////////
 //
 public:
	PNGCodecRGB24();
	virtual ~PNGCodecRGB24();

	static ImageRGB24* readPNG(const char *filename);
	static void writePNG(const char *filename, const ImageRGB24& img);

 //
 ////////////////////////////////////////////////////
 // PROTECTED MEMBERS
 ////////////////////////////////////////////////////
 //
 protected:
	static void initPNGStructs(FILE *pngFP, png_structp &png, png_infop &pngInfo, int bForReading);
	static ImageRGB24* readPNGIntoImage(png_structp &png, png_infop &pngInfo);
	static void writeImageToPNG(const ImageRGB24& img, png_structp &png, png_infop &pngInfo);

 //
 ////////////////////////////////////////////////////
 // MEMBER VARIABLES
 ////////////////////////////////////////////////////
 //
 protected:
};

#endif	// _H_PNGCODECRGB24

