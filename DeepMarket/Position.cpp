#include "Position.hpp"

DeepMarket::Position::Position(const CurrencyPairTrack * currencyPair, float initialPrice, float volume) {
	_initialPrice = initialPrice;
	_currencyPair = currencyPair;
	_volume = volume;
}

DeepMarket::Position::~Position() {}

float DeepMarket::Position::initialPrice() const {
	return _initialPrice;
}

const DeepMarket::CurrencyPairTrack * DeepMarket::Position::currencyPair() const {
	return _currencyPair;
}

DeepMarket::PositionState DeepMarket::Position::state() const {
	return _state;
}

float DeepMarket::Position::volume() const {
	return _volume;
}
