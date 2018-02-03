#pragma once
#include <string>

class Trade {
public:
	static const unsigned int BUY = 0;
	static const unsigned int SELL = 1;
	Trade(const unsigned int type, std::string& symbol, float initialPrice, float stopLoss, float takeProfit);
	~Trade();
	unsigned int type() const;
	std::string symbol() const;
	float initialPrice() const;
	float stopLoss() const;
	float takeProfit() const;
	float profit() const;
private:
	unsigned int _type;
	std::string _symbol;
	float _initialPrice;
	float _stopLoss;
	float _takeProfit;
};