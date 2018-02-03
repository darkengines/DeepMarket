#include "LongPosition.h"

DeepMarket::LongPosition::LongPosition(const CurrencyPairTrack * currencyPair, float initialPrice, float volume) : Position(currencyPair, initialPrice, volume) {}

float DeepMarket::LongPosition::pipsProfit() const {
	return _currencyPair->current().bid - _initialPrice;
}

float DeepMarket::LongPosition::baseProfit() const {
	return (_volume * pipsProfit()) / _currencyPair->current().bid;
}

float DeepMarket::LongPosition::quoteProfit() const {
	return _volume * pipsProfit();
}