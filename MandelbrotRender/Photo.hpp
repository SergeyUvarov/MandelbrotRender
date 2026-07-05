#pragma once

#include <Windows.h>
#include <cstdio>
#include <thread>
#include <functional>
#include <iostream>

#define sRGB(R,G,B) RGB(B,G,R)
#define getR(X) GetBValue(X)
#define getG(X) GetGValue(X)
#define getB(X) GetRValue(X)

class Photo
{
public:
    size_t width;
	size_t height;
    COLORREF *pixels;
	BITMAPINFOHEADER bmInfoHeader;
	BITMAPINFO dbmi;

    Photo(size_t ulWidth, size_t ulHeight): width(ulWidth), height(ulHeight)
    {
        pixels = new COLORREF[width*height];

		bmInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmInfoHeader.biWidth = width;
		bmInfoHeader.biHeight = height;
		bmInfoHeader.biPlanes = 1;
		bmInfoHeader.biBitCount = 32;
		bmInfoHeader.biCompression = BI_RGB;

		dbmi.bmiHeader = bmInfoHeader;
		dbmi.bmiColors->rgbBlue = 0;
		dbmi.bmiColors->rgbGreen = 0;
		dbmi.bmiColors->rgbRed = 0;
		dbmi.bmiColors->rgbReserved = 0;
    }

	void recreate(size_t ulWidth, size_t ulHeight)
	{
		delete[] pixels;
		width = (ulWidth);
		height = (ulHeight);

		pixels = new COLORREF[width*height];

		bmInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmInfoHeader.biWidth = width;
		bmInfoHeader.biHeight = height;
		bmInfoHeader.biPlanes = 1;
		bmInfoHeader.biBitCount = 32;
		bmInfoHeader.biCompression = BI_RGB;

		dbmi.bmiHeader = bmInfoHeader;
		dbmi.bmiColors->rgbBlue = 0;
		dbmi.bmiColors->rgbGreen = 0;
		dbmi.bmiColors->rgbRed = 0;
		dbmi.bmiColors->rgbReserved = 0;
	}

    void setPixel(const int x, const int y, const COLORREF pixColor)
    {
		if (x >= 0 && x < width && y >= 0 && y < height)
        {
			pixels[x + (height - y - 1) * width] = pixColor;
        }
    }

	COLORREF getPixel(unsigned long x, unsigned long y)
	{
		if (x >= 0 && x < width && y >= 0 && y < height)
		{
			return pixels[x + (height - y - 1)*(width)];
		}
		return sRGB(0, 0, 0);
	}

	void clearing(COLORREF background = 0x000000)
	{
		for (unsigned long x = 0; x < width; x++)
			for (unsigned long y = 0; y < height; y++)
				setPixel(x, y, background);
	}

	HBITMAP draw(HDC hdc)
	{
		return CreateDIBitmap(hdc, &bmInfoHeader, CBM_INIT, pixels, &dbmi, DIB_RGB_COLORS);
	}

    void save(const char* file_title)
    {
        FILE *photoFile = nullptr;
        BITMAPFILEHEADER bmFileHeader = {};
        DWORD dwBmpSize = NULL;

        HBITMAP hBitmap = CreateCompatibleBitmap(NULL, width, height);
        BITMAP bmpPhoto = {};

        GetObject(hBitmap, sizeof(BITMAP), &bmpPhoto);

        dwBmpSize = ((width * bmInfoHeader.biBitCount + 31) / 32) * 4 * height;

        DWORD dwSizeofFile = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
        bmFileHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
        bmFileHeader.bfSize = dwSizeofFile;
        bmFileHeader.bfType = 0x4D42; //BM

        fopen_s(&photoFile, file_title, "wb");
        if(photoFile != nullptr)
        {
            fwrite(&bmFileHeader, sizeof(BITMAPFILEHEADER), 1, photoFile);
            fwrite(&bmInfoHeader, sizeof(BITMAPINFOHEADER), 1, photoFile);
            fwrite(pixels, sizeof(COLORREF), width*height, photoFile);
            fclose(photoFile);
        }
		DeleteObject(hBitmap);
    }

	void fillRect(int32_t x, int32_t y, int32_t rectSize, COLORREF color)
	{
		for (int32_t dy = 0; dy < rectSize; dy++)
		{
			for (int32_t dx = 0; dx < rectSize; dx++)
			{
				if (x + dx < width && y + dy < height) 
					setPixel(x+dx, y+dy, color);
			}
		}
	}

	Photo& operator = (Photo& pht)
	{
		memcpy(pixels, pht.pixels, sizeof(COLORREF) * min((height*width), (pht.height * pht.width)));
		return *this;
	}

    ~Photo()
    {
        delete[] pixels;
    }
};
