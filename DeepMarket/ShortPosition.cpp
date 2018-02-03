#include "ShortPosition.h"

DeepMarket::ShortPosition::ShortPosition(const CurrencyPairTrack * currencyPair, float initialPrice, float volume) : Position(currencyPair, initialPrice, volume) {}

float DeepMarket::ShortPosition::pipsProfit() const {
	return _initialPrice - _currencyPair->current().ask;
}

float DeepMarket::ShortPosition::baseProfit() const {
	return (_volume * pipsProfit()) / _currencyPair->current().ask;
}

float DeepMarket::ShortPosition::quoteProfit() const {
	return _volume * pipsProfit();
}
