#pragma once
#include "Position.hpp"
#include "CurrencyPairTrack.hpp"

namespace DeepMarket {
	class ShortPosition : public Position {
	public:
		ShortPosition(const CurrencyPairTrack * currencyPair, float initialPrice, float volume);
		float pipsProfit() const;
		float baseProfit() const;
		float quoteProfit() const;
	};
}