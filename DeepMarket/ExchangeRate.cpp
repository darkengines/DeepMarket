#include "ExchangeRate.hpp"

namespace DeepMarket {
	ExchangeRate::ExchangeRate(const CurrencyPair & currencyPair, FILETIME time, float ask, float bid):
		_currencyPair(currencyPair), _time(time), _ask(ask), _bid(bid)
	{}

	ExchangeRate::ExchangeRate(const ExchangeRate & exchangeRate):
		_currencyPair(exchangeRate._currencyPair), _time(exchangeRate._time), _ask(exchangeRate._ask), _bid(exchangeRate._bid)
	{}

	ExchangeRate::~ExchangeRate() {}

	ExchangeRate & ExchangeRate::operator=(const ExchangeRate & exchangeRate) {
		_currencyPair = exchangeRate._currencyPair;
		_time = exchangeRate._time;
		_ask = exchangeRate._ask;
		_bid = exchangeRate._bid;
		return *this;
	}

	CurrencyPair ExchangeRate::currencyPair() const {
		return _currencyPair;
	}

	FILETIME ExchangeRate::time() const {
		return _time;
	}

	float ExchangeRate::ask() const {
		return _ask;
	}

	float ExchangeRate::bid() const {
		return _bid;
	}

}