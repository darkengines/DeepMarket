#include "CurrencyPairTrack.hpp"

namespace DeepMarket {

	CurrencyPairTrack::CurrencyPairTrack(const CurrencyPair & currencyPair, const std::string & dataFilePath) :
		_currencyPair(currencyPair) {
		_dataFile = fopen(dataFilePath.c_str(), "rb");
		struct stat dataFileStat;
		stat(dataFilePath.c_str(), &dataFileStat);
		_ticksCount = dataFileStat.st_size / sizeof(price_info);
	}

	CurrencyPairTrack::CurrencyPairTrack(const CurrencyPairTrack & currencyPairTrack) :
		_currencyPair(currencyPairTrack._currencyPair) {

	}

	CurrencyPairTrack & CurrencyPairTrack::operator=(const CurrencyPairTrack & currencyPairTrack) {
		return *this;
	}

	CurrencyPairTrack::~CurrencyPairTrack() {
		if (_dataFile) {
			fclose(_dataFile);
			_dataFile = 0;
		}
	}

	boost::signals2::scoped_connection CurrencyPairTrack::connect(const boost::signals2::signal<void(const price_info&)>::slot_function_type  & slot) {
		return _valueChanged.connect(slot);
	}

	const price_info& CurrencyPairTrack::current() const {
		return _current;
	}

	const price_info& CurrencyPairTrack::pending() const {
		return _pending;
	}

	unsigned long CurrencyPairTrack::ticksCount() const {
		return _ticksCount;
	}

	void CurrencyPairTrack::_onValueChanged(const price_info & priceInfo) {
		_valueChanged(priceInfo);
	}

	void CurrencyPairTrack::seek(const SYSTEMTIME time, int from, int to) {
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
		step();
	}
	void CurrencyPairTrack::step() {
		_current = _pending;
		_onValueChanged(_current);
		fread(&_pending, sizeof(price_info), 1, _dataFile);
	}
	CurrencyPair CurrencyPairTrack::currencyPair() const {
		return _currencyPair;
	}
}