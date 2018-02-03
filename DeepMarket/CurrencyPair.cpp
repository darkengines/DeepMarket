#include "CurrencyPair.hpp"

namespace DeepMarket {
	CurrencyPair::CurrencyPair(const Currency& baseCurrency, const Currency& quoteCurrency) :
		_baseCurrency(baseCurrency),
		_quoteCurrency(quoteCurrency) {}
	CurrencyPair::CurrencyPair(const CurrencyPair & currencyPair) :
		_baseCurrency(currencyPair._baseCurrency),
		_quoteCurrency(currencyPair._quoteCurrency) {}
	CurrencyPair & CurrencyPair::operator=(const CurrencyPair & currencyPair) {
		_baseCurrency = currencyPair._baseCurrency;
		_quoteCurrency = currencyPair._quoteCurrency;
		return *this;
	}
	bool CurrencyPair::operator==(const CurrencyPair & currencyPair) {
		return _baseCurrency == currencyPair._baseCurrency
			&& _quoteCurrency == currencyPair._quoteCurrency;
	}
	std::string CurrencyPair::toString() {
		return _baseCurrency.name() + _quoteCurrency.name();
	}
}
