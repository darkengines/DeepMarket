#include "Account.hpp"
#include <vector>
#include <numeric>
#include <functional>

DeepMarket::Account::Account(Currency * currency, int initialDeposit) {
	_currency = currency;
	_initialDeposit = initialDeposit;
}

DeepMarket::Currency * DeepMarket::Account::currency() const {
	return _currency;
}

int DeepMarket::Account::balance() const {
	return _initialDeposit;
}

float DeepMarket::Account::equity() const {
	auto result = std::accumulate(_positions.begin(), _positions.end(), _initialDeposit, [this](float equity, Position* position)->float {
		return equity + position->baseProfit();
	});
	return result;
}

int DeepMarket::Account::leverage() const {
	return _leverage;
}

void DeepMarket::Account::addPosition(Position * position) {
	_positions.push_back(position);
}
