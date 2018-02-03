#pragma once
#include "Currency.hpp"

namespace DeepMarket {
	class CurrencyPair {
	public:
		CurrencyPair(const Currency& baseCurrency, const Currency& quoteCurrency);
		CurrencyPair(const CurrencyPair& currencyPair);
		CurrencyPair& operator=(const CurrencyPair& currencyPair);
		bool operator==(const CurrencyPair& currencyPair);
		std::string toString();
	private:
		Currency _baseCurrency;
		Currency _quoteCurrency;
	};
}