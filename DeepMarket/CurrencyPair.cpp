#include "CurrencyPair.hpp"

namespace DeepMarket {

	CurrencyPair::CurrencyPair(const std::string & name, const std::string & dataFilePath) {
		_name = name;
		_dataFile = fopen(dataFilePath.c_str(), "rb");
		struct stat dataFileStat;
		stat(dataFilePath.c_str(), &dataFileStat);
		_ticksCount = dataFileStat.st_size / sizeof(price_info);
	}

	DeepMarket::CurrencyPair::CurrencyPair(const CurrencyPair & currencyPair) {

	}

	DeepMarket::CurrencyPair::~CurrencyPair() {
		if (_dataFile) {
			fclose(_dataFile);
			_dataFile = 0;
		}
	}

	boost::signals2::scoped_connection DeepMarket::CurrencyPair::connect(const boost::signals2::signal<void(const price_info&)>::slot_function_type  & slot) {
		return _valueChanged.connect(slot);
	}

	const price_info& DeepMarket::CurrencyPair::current() const {
		return _current;
	}

	const price_info& DeepMarket::CurrencyPair::pending() const {
		return _pending;
	}

	std::string CurrencyPair::name() const {
		return _name;
	}

	unsigned long CurrencyPair::ticksCount() const {
		return _ticksCount;
	}

	void DeepMarket::CurrencyPair::_onValueChanged(const price_info & priceInfo) {
		_valueChanged(priceInfo);
	}

	void CurrencyPair::seek(const SYSTEMTIME time, int from, int to) {
		if (to > from) {
			auto middle = (int)floor((to + from) / 2);
			fseek(_dataFile, middle * sizeof(price_info), 0);
			fread(&_pending, sizeof(price_info), 1, _dataFile);
			fseek(_dataFile, middle * sizeof(price_info), 0);
			FILETIME seekTime;
			SystemTimeToFileTime(&time, &seekTime);
			auto cmp = CompareFileTime(&seekTime, &_pending.time);
			if (cmp < 0) seek(time, from, middle - 1);
			if (cmp > 0) seek(time, middle + 1, to);
		}
	}
	void CurrencyPair::step() {
		_current = _pending;
		_onValueChanged(_current);
		fread(&_pending, sizeof(price_info), 1, _dataFile);
	}
}