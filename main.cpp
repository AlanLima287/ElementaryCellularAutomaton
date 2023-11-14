#include "window.h"

using namespace Window;

uchar ruleNum = 0;
bool mode = true;

int main() {
	Window::WinStart(L"Rule Renderer");
	return 0;
}

bool getPixelEdgeCase(int x, int y) {
	if (x < 0) return ctx::getPixel(x + 1, y) == 0xffffff;
	if (x >= Window::window.width) return ctx::getPixel(x - 1, y) == 0xffffff;
	return ctx::getPixel(x, y) == 0xffffff;
}

void cellularAutomaton(uchar rule) {

	ctx::fillPixel(window.midPointX, window.height - 1, 0xffffff);

	int endHeight = window.height - window.midPointX;
	if (endHeight < 0) endHeight = 0;

	for (int y = window.height - 1; y > endHeight; y--) {
		for (int x = 0; x < window.width; x++) {

			uchar top = 0;
			if (getPixelEdgeCase(x - 1, y)) top |= 0b100;
			if (getPixelEdgeCase(x, y))		top |= 0b010;
			if (getPixelEdgeCase(x + 1, y)) top |= 0b001;

			if ((rule & (1 << top)) > 0) ctx::fillPixel_s(x, y - 1, 0xffffff);
		}
	}
}

void renderRule(uchar rule) {

	ctx::fillBackground(0);
	cellularAutomaton(rule);

	char nums[2][9] = {};
	sprintf_s(nums[0], "%d", rule);
	for (char i = 0; i < 8; i++) nums[1][7 - i] = ((ruleNum & (1 << i)) > 0) ? '1' : '0';
	ctx::fillText(10, window.height - 10, 32, nums[1], 0x777777, 0, 0);
	ctx::fillText(10, window.height - 42, 32, nums[0], 0x777777, 0, 0);
}

/* Window Overloads */

void setupApplication() {
	renderRule(ruleNum);
}

void simulateApplication(float deltaTime) {

	if (pressed(VK_SHIFT)) ruleNum = 0;

	if (isDown(VK_SHIFT)) {

		ctx::fillBackground(0);
		if (pressed(VK_DOWN) || pressed(VK_UP)) mode = !mode;

		if (mode) {
			for (char i = '0'; i <= '9'; i++) if (pressed(i)) ruleNum = ruleNum * 10 + (i - '0');
			if (pressed(VK_BACK)) ruleNum /= 10;
		}
		else {
			if (pressed('0')) ruleNum <<= 1;
			if (pressed('1')) { ruleNum <<= 1; ruleNum |= 0b1; }
			if (pressed(VK_BACK)) ruleNum >>= 1;
		}

		char nums[2][9] = {};
		sprintf_s(nums[0], "%d", ruleNum);
		for (char i = 0; i < 8; i++) nums[1][7 - i] = ((ruleNum & (1 << i)) > 0) ? '1' : '0';

		ctx::fillText(window.midPointX, window.height, 32, nums[0], (mode) ? 0xffffff : 0x777777, ctx::ALIGN_CENTER);
		ctx::fillText(window.midPointX, window.height - 32, 32, nums[1], (!mode) ? 0xffffff : 0x777777, ctx::ALIGN_CENTER);
	}

	if (released(VK_SHIFT)) renderRule(ruleNum);

	if (released(VK_OEM_PLUS) || released(VK_OEM_MINUS)) {
		released(VK_OEM_PLUS) ? ruleNum++ : ruleNum--;
		renderRule(ruleNum);
	}
}

void onResize() {
	renderRule(ruleNum);
}
