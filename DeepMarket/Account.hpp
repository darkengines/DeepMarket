#pragma once
#include "Currency.hpp"
#include "Position.hpp"

namespace DeepMarket {
	class Account {
	public:
		Account(Currency* currency, int initialDeposit);
		Currency* currency() const;
		int balance() const;
		float equity() const;
		int leverage() const;
		void addPosition(Position* position);
	protected:
		Currency* _currency;
		int _initialDeposit;
		int _leverage;
		std::vector<Position*> _positions;
	};
}