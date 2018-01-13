#pragma once
#include <Windows.h>

namespace DeepMarket {
	typedef struct {
	public:
		FILETIME time;
		float ask;
		float bid;
	} price_info;
}