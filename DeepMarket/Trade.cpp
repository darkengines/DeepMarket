#include "Trade.hpp"

Trade::Trade(const unsigned int type, std::string & symbol, float initialPrice, float stopLoss, float takeProfit) {
	_type = type;
	_symbol = symbol;
	_initialPrice = initialPrice;
	_stopLoss = stopLoss;
	_takeProfit = takeProfit;
}

Trade::~Trade() {}

unsigned int Trade::type() const {
	return _type;
}

std::string Trade::symbol() const {
	return _symbol;
}

float Trade::initialPrice() const {
	return _initialPrice;
}

float Trade::stopLoss() const {
	return _stopLoss;
}

float Trade::takeProfit() const {
	return _takeProfit;
}

float Trade::profit() const {
	auto pips = _type == Trade::BUY ? _symbol
}
