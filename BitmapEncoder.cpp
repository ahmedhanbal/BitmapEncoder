/// Only on Windows
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <windows.h> // for shell calls

typedef char color; // just for fun

struct Bitmap
{ // will store entire PixelData

	// not to be confused with width and height of pixles array
	int width;	// pixelsX
	int height; // pixelsY
	int padding; // padding to cater 4 byte boundary
	int pixelDataSize; // total pixel data size in bytes
	color *pixels;

	Bitmap(int w, int h)
	{
		width = w;
		height = h;
		padding = (4 - (width * 3) % 4) % 4;
		pixelDataSize = (width * 3 + padding) * height;
		pixels = new color[pixelDataSize];
	}

	~Bitmap()
	{
		delete[] pixels;
	}
};

int writeFileHeader(FILE *&file, Bitmap &bitmap) // writes BMP File Header 14 bytes in File
{												 // Writing FileHeader of .BMP
	int pixelDataSize = bitmap.pixelDataSize;
	int fileSize = 14 + 40 + pixelDataSize;
	short reserved = 0;
	int offset = 54;
	fputs("BM", file);
	fwrite(&fileSize, sizeof(int), 1, file);
	fwrite(&reserved, sizeof(short), 1, file);
	fwrite(&reserved, sizeof(short), 1, file);
	fwrite(&offset, sizeof(int), 1, file);
	return 0;
}

int writeInfoHeader(FILE *&file, Bitmap &bitmap) // Writes BMP Info Header 40 bytes in File
{												 // Writing Info Header of .BMP
	int size = 40;
	short biPlanes = 1;
	short bitCountPerPixel = 24;
	int compression = 0;
	int pixelDataSize = bitmap.pixelDataSize;
	int zero = 0; // zeros for resX,resY,color used, important color as we do not use them here
	fwrite(&size, sizeof(int), 1, file);
	fwrite(&bitmap.width, sizeof(int), 1, file);
	fwrite(&bitmap.height, sizeof(int), 1, file);
	fwrite(&biPlanes, sizeof(short), 1, file);
	fwrite(&bitCountPerPixel, sizeof(short), 1, file);
	fwrite(&compression, sizeof(int), 1, file);
	fwrite(&pixelDataSize, sizeof(int), 1, file);

	// zeros for resX,resY,color used, important color as we do not use them here
	for (int i = 0; i < 4; i++)
		fwrite(&zero, sizeof(int), 1, file);

	return 0;
}

void writePixelData(FILE *file, Bitmap &bitmap) // Write Pixel Data in File
{												// Writing Random rgb values in PixelDatas
	int width = bitmap.width;					// to reduce access times
	for (int y = bitmap.height - 1; y >= 0; y--)
	{
		for (int x = 0; x < bitmap.width; x++)
		{
			fwrite(&bitmap.pixels[y * width + x], sizeof(int), 1, file);
		}
	}
}

void generateBitmap(Bitmap &bitmap) // Initializes Pixels, and what to store in them. Change this to have your desired output
{
	int color = 2; // 0 for blue, 1 for green, 2 for red

	for (int y = bitmap.height - 1; y >= 0; y--)
	{
		for (int x = color; x < bitmap.width * 3; x+=3) // since colors are stored as bgr(0,0,0), start from 0 to give only blue, 1 to give only green and 2 for red
		{
			bitmap.pixels[y*bitmap.width+x] = 255;
		}
	}
}

void write(Bitmap &bitmap, const char *&filename)
{
	FILE *file = fopen(filename, "wb");

	if (!file)
	{
		return;
	}

	// generate pixeldata of bitmap
	generateBitmap(bitmap);
	
	// calling fns to write multiple parts of file
	writeFileHeader(file, bitmap);
	writeInfoHeader(file, bitmap);
	writePixelData(file, bitmap);

	fclose(file);
}

int main()
{
	srand(0);
	int width = 0, height = 0;
	printf("Enter width of Bitmap to produce (pixels) : ");
	scanf_s("%d", &width);
	printf("Enter height of Bitmap to produce (pixels) : ");
	scanf_s("%d", &height);

	Bitmap bitmap(width, height);
	const char *filename = "BinEncoder.bmp";
	write(bitmap, filename);

	// opening in default viewer
	if ((unsigned long long)ShellExecuteA(NULL, "open", filename, NULL, NULL, SW_SHOWNORMAL) > 32)
		printf("\nOpened file in Default Bitmap Viewer successfully\n");
	else
		printf("!!!Failed to Open File %s", filename);

	return 0;
}
