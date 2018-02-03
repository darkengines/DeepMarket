#pragma once
#include <Windows.h>
#include "CurrencyPair.hpp"

namespace DeepMarket {
	class ExchangeRate {
	public:
		ExchangeRate(const CurrencyPair& currencyPair, FILETIME time, float ask, float bid);
		ExchangeRate(const ExchangeRate& exchangeRate);
		~ExchangeRate();
		ExchangeRate& operator=(const ExchangeRate& exchangeRate);
		CurrencyPair currencyPair() const;
		FILETIME time() const;
		float ask() const;
		float bid() const;
	private:
		CurrencyPair _currencyPair;
		FILETIME _time;
		float _ask;
		float _bid;
	};
}