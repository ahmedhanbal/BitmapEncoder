/// Only on Windows
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <windows.h> // for shell calls

typedef int color; // just for fun

struct Bitmap { // will store entire PixelData
	int width;
	int height;
	color* pixels;

	Bitmap(int w, int h) {
		width = w;
		height = h;
		pixels = new int[width * height];
	}

	~Bitmap() {
		delete[] pixels;
	}
};

int writeFileHeader(FILE*& file, Bitmap& bitmap) {  // Writing FileHeader of .BMP
	int pixelDataSize = (bitmap.width * 3 + (4 - (bitmap.width * 3) % 4) % 4) * bitmap.height;
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

int writeInfoHeader(FILE*& file, Bitmap& bitmap) { // Writing Info Header of .BMP
	int size = 40;
	short biPlanes = 1;
	short bitCountPerPixel = 24;
	int compression = 0;
	int pixelDataSize = (bitmap.width * 3 + (4 - (bitmap.width * 3) % 4) % 4) * bitmap.height;
	int zero = 0; // zeros for resX,resY,color used, important color as we do not use them here
	fwrite(&size, sizeof(int), 1, file);
	fwrite(&bitmap.width, sizeof(int), 1, file);
	fwrite(&bitmap.height, sizeof(int), 1, file);
	fwrite(&biPlanes, sizeof(short), 1, file);
	fwrite(&bitCountPerPixel, sizeof(short), 1, file);
	fwrite(&compression, sizeof(int), 1, file);
	fwrite(&pixelDataSize, sizeof(int), 1, file);
	fwrite(&zero, sizeof(int), 1, file);
	fwrite(&zero, sizeof(int), 1, file);
	fwrite(&zero, sizeof(int), 1, file);
	fwrite(&zero, sizeof(int), 1, file);
	return 0;
}

void writeBitmapData(FILE* file, Bitmap& bitmap) { // Writing Random rgb values in PixelData
	int padding = (4 - (bitmap.width * 3) % 4) % 4;
	unsigned char paddingData[3] = { 0, 0, 0 };
	int index;
	for (int y = bitmap.height - 1; y >= 0; y--) {
		for (int x = 0; x < bitmap.width; x++) {
			unsigned char color[3] = { 0, 0, 0 }; // for rgb values
			for (int i = 0; i < 3; i++) {
				color[i] = rand() % 255; // random
			}
			fwrite(color, sizeof(unsigned char), 3, file);
		}
		fwrite(paddingData, sizeof(unsigned char), padding, file);
	}
}

void write(Bitmap& bitmap, const char*& filename) {
	FILE* file = nullptr;

	if (fopen_s(&file, filename, "wb") != 0) {
		return;
	}

	// calling fns to write multiple parts of file
	writeFileHeader(file, bitmap);
	writeInfoHeader(file, bitmap);
	writeBitmapData(file, bitmap);

	fclose(file);
}

int main() {
	srand(0);
	int width = 0, height = 0;
	printf("Enter width of Bitmap to produce (pixels) : ");
	scanf_s("%d", &width);
	printf("Enter height of Bitmap to produce (pixels) : ");
	scanf_s("%d", &height);

	Bitmap bitmap(width, height);
	const char* filename = "BinEncode.bmp";
	write(bitmap, filename);

	// opening in default viewer
	if ((unsigned long long) ShellExecuteA(NULL, "open", filename, NULL, NULL, SW_SHOWNORMAL) > 32)
		printf("\nOpened file in Default Bitmap Viewer successfully\n");
	else
		printf("!!!Failed to Open File %s", filename);

	return 0;
}
