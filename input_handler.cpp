#pragma once

typedef unsigned char uchar;
static const uchar KEYBOARD_BUTTON_COUNT = 0xff;

struct ButtonState {
	bool changed, isDown;
};

struct MouseState {
	ButtonState right, middle, left;
	int offsetX, offsetY;
};

struct Input {
	ButtonState keyboard[KEYBOARD_BUTTON_COUNT];
	uchar lastPressedKey = 0;
	MouseState mouse;
};

#define isDown(b) input.keyboard[b].isDown
#define pressed(b) (input.keyboard[b].isDown && input.keyboard[b].changed)
#define released(b) (!input.keyboard[b].isDown && input.keyboard[b].changed)

#define mouseIsDown(s) input.mouse.s.isDown
#define mousePressed(s) (input.mouse.s.isDown && input.mouse.s.changed)
#define mouseReleased(s) (!input.mouse.s.isDown && input.mouse.s.changed)