#pragma once

#include <random>

#define clamp(min, val, max) ((val < min) ? min : ((val > max) ? max : val))

inline int mod(int num, int mod) {
	int result = num % mod;
	return (result < 0) ? result + mod : result;
}

inline float random() {
	return (float)rand() / (RAND_MAX + 1);
};