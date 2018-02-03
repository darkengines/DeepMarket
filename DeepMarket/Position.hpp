#pragma once
#include "CurrencyPairTrack.hpp"

namespace DeepMarket {

	enum PositionState { Opened, Closed };

	class Position {
	public:
		Position(const CurrencyPairTrack * currencyPair, float initialPrice, float volume);
		~Position();
		float initialPrice() const;
		const CurrencyPairTrack * currencyPair() const;
		virtual float pipsProfit() const = 0;
		virtual float baseProfit() const = 0;
		virtual float quoteProfit() const = 0;
		PositionState state() const;
		float volume() const;
	protected:
		const CurrencyPairTrack * _currencyPair;
		float _initialPrice;
		PositionState _state;
		float _volume;
	};
}