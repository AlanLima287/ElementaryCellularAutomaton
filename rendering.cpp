#pragma once

#include <Windows.h>

#include "font_map.cpp"
#include "utils.h"

struct WindowState {
	void* buffer;
	BITMAPINFO bitmapInfo;
	int screenHeight, screenWidth;
	int midPointX, midPointY;
	int height, width;
};

namespace ctx {
	
	typedef unsigned char uchar;
	typedef unsigned long u32;

	WindowState* window;
	FontMap::Font font;
	
	const uchar ALIGN_START = 0;
	const uchar ALIGN_CENTER = 1;
	const uchar ALIGN_END = 2;

	void setWindow(WindowState* wnd) {
		window = wnd;
	}

	u32 getPixel(int x, int y) {
		return *((u32*)window->buffer + x + y * window->width);
	}

	u32 getPixel_s(int x, int y) {
		if (0 <= x && x < window->width && 0 <= y && y < window->height)
			return *((u32*)window->buffer + x + y * window->width);
		else return 0;
	}

	void fillPixel(int x, int y, u32 color) {
		u32* pixel = (u32*)window->buffer + x + y * window->width;
		*pixel = color;
	}

	bool fillPixel_s(int x, int y, u32 color) {
		if (0 <= x && x < window->width && 0 <= y && y < window->height) {
			u32* pixel = (u32*)window->buffer + x + y * window->width;
			*pixel = color;
			return 1;
		}

		return 0;
	}

	void fillBackground(u32 color) {
		u32* pixel = (u32*)window->buffer;
		for (int x = 0; x < window->width; x++) {
			for (int y = 0; y < window->height; y++) {
				*pixel++ = color;
			}
		}
	}

	void fillRect(int startX, int startY, int sizeX, int sizeY, u32 color) {

		sizeX = startX + sizeX;
		sizeY = startY + sizeY;

		startX = clamp(0, startX, window->width);
		startY = clamp(0, startY, window->height);
		sizeX = clamp(0, sizeX, window->width);
		sizeY = clamp(0, sizeY, window->height);

		for (int y = startY; y < sizeY; y++) {
			u32* pixel = (u32*)window->buffer + startX + y * window->width;
			for (int x = startX; x < sizeX; x++) *pixel++ = color;
		}
	}

	void drawLine(int startX, int startY, int endX, int endY, u32 color) {

		int dx = abs((int)(endX - startX));
		int dy = -abs((int)(endY - startY));
		int signX = startX < endX ? 1 : -1;
		int signY = startY < endY ? 1 : -1;
		int error = dx + dy;

		while (true) {

			fillPixel_s(startX, startY, color);
			if (startX == endX && startY == endY) break;

			int e2 = error << 1;

			if (e2 >= dy) {

				startX = startX + signX;
				error = error + dy;

			} if (e2 <= dx) {

				startY = startY + signY;
				error = error + dx;
			}
		}
	}

	void fillText(int x, int y, int fontSize, const char* text, u32 color, char hAlign = ALIGN_CENTER, char vAlign = ALIGN_START, int padding = 0) {

		if (!font.isLoaded) return;

		float stepSize = (float)fontSize / font.height;
		int blockSize = (int)round(stepSize);

		stepSize = font.width * stepSize + padding;

		float posX = (hAlign == 0) ? x : (float)x - (((int)strlen(text) * stepSize - padding) * hAlign * .5f);
		float posY = (vAlign == 0) ? y : y + fontSize * vAlign * .5f;
		float pivotX = posX;
		float pivotY = posY;

		while (*text) {

			uchar* charSheet = FontMap::getCharacterSheet(font, *text);

			posX = pivotX;
			posY = pivotY;
			int index = -1;
			int pointer = 128;

			for (int i = 0; i < font.length; i++) {
				if (i % 8 == 0) { index++; }
				if (i % font.width == 0) { posY -= blockSize; posX = pivotX; }
				if ((charSheet[index] & pointer) > 0) fillRect((int)posX, (int)posY, blockSize, blockSize, color);
				pointer = pointer == 1 ? 128 : pointer >> 1;
				posX += blockSize;
			}

			pivotX += stepSize;
			text++;
		}
	}
};