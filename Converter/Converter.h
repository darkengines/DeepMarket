#pragma once
#include <iostream>
#include "../DeepMarket/price_info.h"

class Converter {
public:
	void convert(const char* inputDir, const char* outputDir, bool(*predicate)(int, int) = NULL, int numThreads = 8);
	int readFile(const char* inputFile, DeepMarket::price_info** outPriceInfos, std::size_t* outPriceInfosCount);
	void convertPairs(const char* inputDir, const char* outputDir, bool(*predicate)(int, int) = NULL);
};
