#pragma once
#include "../DeepMarket/price_info.h"

typedef struct read_file_result {
	DeepMarket::price_info* price_infos;
	std::size_t price_infos_count;
	int order;
} read_file_result_t;