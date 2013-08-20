#include <PNGCodecRGB24.h>



//
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//
// class ImageRGB24
//
// Purpose:
//  Basic struct for images
//
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
//
ImageRGB24::ImageRGB24(int inWidth, int inHeight, int inDepth) 
{
	if (inWidth <= 0)
		throw new std::runtime_error("Width must be positive");
	if (inHeight <= 0)
		throw new std::runtime_error("Height must be positive");
	if (inDepth != 3)
		throw new std::runtime_error("Depth must be 3 - only 24-bit RGB images supported in this version");

	width = inWidth;
	height = inHeight;
	depth = inDepth;

	// Initialise memory - allocate row pointers, then allocate each row
	data = new RGB24*[height];
	for (int y = 0; y < height; y++) {
		data[y] = new RGB24[width * depth];
		memset(data[y], 0, width * depth);	// Init to zeros
	}
}

ImageRGB24::~ImageRGB24()
{
	if (data != NULL) {
		for (int y = 0; y < height; y++) {
			delete [] data[y];
        }
		delete[] data;
		data = NULL;
	}
}



//
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//
// class PNGCodecRGB24
//
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
//


//
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//
// CONSTRUCTORS / DESTRUCTORS
//
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////
//
// PNGCodecRGB24()
//
////////////////////////////////////////////////////
//
PNGCodecRGB24::PNGCodecRGB24()
{
	// Nothing to init - all members are static
}


//
////////////////////////////////////////////////////
//
// ~PNGCodecRGB24()
//
////////////////////////////////////////////////////
//
PNGCodecRGB24::~PNGCodecRGB24()
{
}







//
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//
// PUBLIC MEMBERS
//
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////
//
// void readPNG()
//
////////////////////////////////////////////////////
//
ImageRGB24* PNGCodecRGB24::readPNG(const char *filename)
{
	FILE *pngFP;
	png_structp png;
	png_infop pngInfo;
	ImageRGB24* img;

	if (filename == NULL)
		throw new std::runtime_error("PNGCodecRGB24::readPNG(): filename must not be NULL");


	if ((pngFP = fopen(filename, "rb")) == NULL)
		throw new std::runtime_error("PNGCodecRGB24::readPNG(): file not found");

	initPNGStructs(pngFP, png, pngInfo, 1);

	/* If we have already read some of the signature */
	///////////////   png_set_sig_bytes(png_ptr, sig_read);

	img = readPNGIntoImage(png, pngInfo);

	return img;
}



//
////////////////////////////////////////////////////
//
// void writePNG()
//
////////////////////////////////////////////////////
//
void PNGCodecRGB24::writePNG(const char *filename, const ImageRGB24& img)
{
	FILE *pngFP;
	png_structp png;
	png_infop pngInfo;

#ifndef _NO_ERR_CHECK
	if (filename == NULL)
		throw new std::runtime_error("PNGCodecRGB24::writePNG(): filename must not be NULL");
#endif

	if ((pngFP = fopen(filename, "wb")) == NULL)
		throw new std::runtime_error("PNGCodecRGB24::writePNG(): file could not be opened");


	initPNGStructs(pngFP, png, pngInfo, 0);

	writeImageToPNG(img, png, pngInfo);
}



//
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//
// PROTECTED MEMBERS
//
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////
//
// void initPNGStructs()
//
////////////////////////////////////////////////////
//
void PNGCodecRGB24::initPNGStructs(FILE *pngFP, png_structp &png, png_infop &pngInfo, int bForReading)
{

	if (pngFP == NULL)
		throw new std::runtime_error("PNGCodecRGB24::initPNGStructs(): pngFP must not be NULL");
	


	if (bForReading)
		png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL); 
	else
		png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL); 
	
	if (png == NULL) {
		fclose(pngFP);
		throw new std::runtime_error("PNGCodecRGB24::initPNGStructs(): Failed to create PNG struct");
	}

	// Allocate/initialize the memory for image information.
	pngInfo = png_create_info_struct(png);
	if (pngInfo == NULL) {
		// Clean up
		fclose(pngFP);
		if (bForReading)
			png_destroy_read_struct(&png, png_infopp_NULL, png_infopp_NULL);
		else
			png_destroy_write_struct(&png, png_infopp_NULL);

		throw new std::runtime_error("PNGCodecRGB24::initPNGStructs(): Failed to create PNG info struct");
	}


   // Set up the input control if using the standard C streams
	png_init_io(png, pngFP);
}




//
////////////////////////////////////////////////////
//
// ImageRGB24* readPNGIntoImage()
//
////////////////////////////////////////////////////
//
ImageRGB24* PNGCodecRGB24::readPNGIntoImage(png_structp &png, png_infop &pngInfo)
{
	unsigned int y;
	png_uint_32 iWidth, iHeight, iRowLength;
	png_byte iColourType, iBitsPerChannel, iNumChannels;
	png_bytepp pngRows;

	ImageRGB24* img;

	// For 8-bit greyscale:	png_read_png(png, pngInfo, PNG_TRANSFORM_IDENTITY, png_voidp_NULL);
	png_read_png(png, pngInfo, PNG_TRANSFORM_BGR | PNG_TRANSFORM_STRIP_ALPHA, png_voidp_NULL);
	
	// pixels are in info_ptr->row_pointers
	// where row_pointers is:
	//   png_bytep row_pointers[height];
	// and is probably not contiguous

	iColourType = png_get_color_type(png, pngInfo);
	// For 32-bit colour:	if (iColourType != PNG_COLOR_TYPE_RGB_ALPHA)
	// For 8-bit greyscale:	if (iColourType != PNG_COLOR_TYPE_GRAY)
	if (iColourType != PNG_COLOR_TYPE_RGB)
		throw new std::runtime_error("PNGCodecRGB24::readPNGIntoImage: Colour type not supported - RGB24 only");



	iBitsPerChannel = png_get_bit_depth(png, pngInfo);
	iNumChannels = png_get_channels(png, pngInfo);
	if (iNumChannels != 3)
		throw new std::runtime_error("PNGCodecRGB24::readPNGIntoImage(): Image format not supported - RGB 24-bit only");

	iHeight = png_get_image_height(png, pngInfo);
	iWidth = png_get_image_width(png, pngInfo);

	iRowLength = iWidth * (iBitsPerChannel * iNumChannels) / 8; 

	img = new ImageRGB24(iWidth, iHeight, iNumChannels);
      pngRows = png_get_rows(png, pngInfo);
	for (int y = 0; y < iHeight; y++) {
		memcpy(img->data[y], pngRows[y], iRowLength);
	}
			

	// Clean up
	png_destroy_read_struct(&png, &pngInfo, png_infopp_NULL);

	return img;
}







//
////////////////////////////////////////////////////
//
// void writeImageIntoPNG(ImageRGB24& img)
//
////////////////////////////////////////////////////
//
void PNGCodecRGB24::writeImageToPNG(const ImageRGB24& img, png_structp& png, png_infop& pngInfo)
{
	unsigned int y;
	png_uint_32 iWidth, iHeight;
	png_bytepp pngRows;

	iWidth = img.width;
	iHeight = img.height;

	// For greyscal images: png_set_IHDR(png, pngInfo, iWidth, iHeight, 8, PNG_COLOR_TYPE_GRAY,
	//			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_set_IHDR(png, pngInfo, iWidth, iHeight, 8, PNG_COLOR_TYPE_RGB,
				PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);



	png_write_info(png, pngInfo);


	/* flip BGR pixels to RGB */
	png_set_bgr(png);


	pngRows = new png_bytep[iHeight];

	// 'Cheat' a little for efficiency - don't actually copy into pngRows, but
	// instead let it re-use img's rows: as the write to PNG won't change the
	// row data, img won't be changed anyways.
	for (y = 0; y < iHeight; y++)
		pngRows[y] = (unsigned char*)img.data[y];

	png_write_image(png, pngRows);

	png_write_end(png, pngInfo);

	// Clean up
	delete [] pngRows;
	png_destroy_write_struct(&png, &pngInfo);

}




