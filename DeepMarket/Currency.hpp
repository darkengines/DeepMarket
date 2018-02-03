#pragma once
#include <string>

namespace DeepMarket {
	class Currency {
	public:
		Currency(const std::string name);
		Currency(const Currency& currency);
		Currency& operator=(const Currency& currency);
		bool operator==(const Currency& currency);
		~Currency();
		const std::string& name() const;
	private:
		std::string _name;
	};
}