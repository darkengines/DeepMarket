#include "Converter.h"

int main(char* argc, char** argv) {
	auto converter = new Converter();
	converter->convertPairs("H:/DeepMarket/GetData/data/", "H:/DeepMarket/GetData/dat/", [](int year, int month)->bool { return year >= 2016; });
	return 0;
}