#pragma once
#include <random>
#include <cstdlib>
#include <vector>
#include <chrono>
#include <cassert>
#include <ctime>

inline double random(double l, double r) {
	static std::default_random_engine generator(
		std::chrono::high_resolution_clock::now().time_since_epoch().count() // seed
	);
	std::uniform_real_distribution<double> distribution(l, r);
	return distribution(generator);
}

inline double random() {
	static std::default_random_engine generator(
		std::chrono::high_resolution_clock::now().time_since_epoch().count() // seed
	);
	static std::uniform_real_distribution<double> distribution(0, 1); //doubles from -1 to 1
	return distribution(generator);
}

template<class T> inline T& getRandomElement(std::vector<T>& V) {
	assert(!V.empty(), "Array in get random element is empty"); // if the array is empty than something is wrong
	return V[rand() % static_cast<int>(V.size())];
}

//Fisher–Yates shuffle
template<class T> inline void shuffle(std::vector<T>& V) {
	int size = V.size();
	while(size) {
		swap(V[rand() % size],V[size-1]);
		size -= 1;
	}
}

