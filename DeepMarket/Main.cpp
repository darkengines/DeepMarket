#include <iostream>
#include "CurrencyPair.hpp"
#include "Market.hpp"

int main(int argc, char** argv) {

	auto market = new DeepMarket::Market("C:/DeepMarket/Data");
	SYSTEMTIME time;
	time.wYear = 2016;
	time.wMonth = 6;
	time.wDay = 15;
	time.wHour = 12;
	time.wMinute = 30;
	time.wSecond = 30;
	time.wMilliseconds = 500;

	int counter = 0;

	auto marketConnection = market->connect([&](const DeepMarket::price_info& priceInfo)->void {
		if (counter % 10000 == 0) {
			auto sout = market->toString();
			auto out = sout.c_str();
			system("cls");
			auto marketTime = market->time();
			FileTimeToSystemTime(&marketTime, &time);
			printf(
				"%02d/%02d/%04d %02d:%02d:%02d.%03d\n%s",
				time.wDay,
				time.wMonth,
				time.wYear,
				time.wHour,
				time.wMinute,
				time.wSecond,
				time.wMilliseconds,
				out
			);
		}
		counter++;
	});

	market->seek(time);
	market->start();

	char x;
	std::cin >> x;

	market->stop();

	return 0;
}