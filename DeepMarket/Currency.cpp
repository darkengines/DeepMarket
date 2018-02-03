#include "Currency.hpp"

namespace DeepMarket {
	Currency::Currency(const std::string name) {
		_name = name;
	}

	Currency::Currency(const Currency & currency) {
		_name = currency._name;
	}

	Currency & Currency::operator=(const Currency & currency) {
		_name = currency._name;
		return *this;
	}

	bool Currency::operator==(const Currency & currency) {
		return _name == currency._name;
	}

	Currency::~Currency() {}

	const std::string & Currency::name() const {
		return _name;
	}

}