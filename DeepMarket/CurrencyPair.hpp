#pragma once
#include <boost/signals2.hpp>
#include <string>
#include <boost/filesystem.hpp>
#include "price_info.h"

namespace DeepMarket {
	class CurrencyPair {
	public:
		CurrencyPair(const std::string& name, const std::string& dataFilePath);
		CurrencyPair(const CurrencyPair& currencyPair);
		~CurrencyPair();
		boost::signals2::scoped_connection connect(const boost::signals2::signal<void(const price_info&)>::slot_function_type & slot);
		const price_info& current() const;
		const price_info& pending() const;
		std::string name() const;
		unsigned long ticksCount() const;
		void seek(const SYSTEMTIME time, int from, int to);
		void step();
	protected:
		boost::signals2::signal<void(const price_info&)> _valueChanged;
		void _onValueChanged(const price_info&);
		std::string _name;
		FILE * _dataFile = 0;
		unsigned long _ticksCount = 0;
		price_info _current;
		price_info _pending;
	};
}