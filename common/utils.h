#pragma once
#include <random>


int randomInt(const int min, const int max) {
	static std::mt19937 rng(std::random_device{}());
	std::uniform_int_distribution<int> dist(min, max);
	return dist(rng);
}

double random(const int min, const int max) {
	static std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<> dist(min, max);
	return dist(rng);
}
