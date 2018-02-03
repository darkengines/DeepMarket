#include "Market.hpp"
#include <boost/filesystem.hpp>
#include <functional>
#include <boost/algorithm/string/join.hpp>

namespace DeepMarket {

	Market::Market(const std::string & dataDirPath) {
		boost::filesystem::directory_iterator directoryIterator(dataDirPath);
		boost::filesystem::directory_iterator iteratorEnd;

		Currency* existingBaseCurrency = 0;
		Currency* existingQuoteCurrency = 0;

		while (directoryIterator != iteratorEnd) {
			if (boost::filesystem::is_regular_file(directoryIterator->path())) {
				boost::filesystem::path path = directoryIterator->path();
				auto name = path.filename().replace_extension("").string();
				auto filePath = path.string();
				auto baseCurrencyName = name.substr(0, 3);
				auto quoteCurrencyName = name.substr(3);

				auto existingBaseCurrencyIterator = std::find_if(_currencies.begin(), _currencies.end(), [baseCurrencyName](Currency* currency)->bool { return currency->name() == baseCurrencyName; });

				if (existingBaseCurrencyIterator == _currencies.end()) {
					existingBaseCurrency = new Currency(baseCurrencyName);
					_currencies.push_back(existingBaseCurrency);
				} else {
					existingBaseCurrency = *existingBaseCurrencyIterator;
				}

				auto existingQuoteCurrencyIterator = std::find_if(_currencies.begin(), _currencies.end(), [quoteCurrencyName](Currency* currency)->bool { return currency->name() == quoteCurrencyName; });

				if (existingQuoteCurrencyIterator == _currencies.end()) {
					existingQuoteCurrency = new Currency(quoteCurrencyName);
					_currencies.push_back(existingQuoteCurrency);
				} else {
					existingQuoteCurrency = *existingQuoteCurrencyIterator;
				}
				CurrencyPair currencyPair(*existingBaseCurrency, *existingQuoteCurrency);
				auto currencyPairTrack = new CurrencyPairTrack(currencyPair, filePath);
				_currencyPairs.push_back(currencyPairTrack);
			}
			directoryIterator++;
		}
		delete existingBaseCurrency;
		delete existingQuoteCurrency;
		_trackCount = _currencyPairs.size();
		_sortedTracks = new CurrencyPairTrack*[_trackCount];
		memcpy(_sortedTracks, _currencyPairs.data(), _trackCount * sizeof(CurrencyPairTrack*));
	}

	Market::Market(const Market & market) {}

	Market::~Market() {
		for (auto &track : _currencyPairs) {
			delete track;
		}
		if (_sortedTracks) delete _sortedTracks;
		if (_run) stop();
	}

	void Market::seek(SYSTEMTIME time) {
		_threadWorkerMutex.lock();
		for (auto &track : _currencyPairs) {
			track->seek(time, 0, track->ticksCount());
		}
		std::sort(_sortedTracks, _sortedTracks + _trackCount, [](const CurrencyPairTrack* a, const CurrencyPairTrack* b)->bool {
			auto aTime = a->pending().time;
			auto bTime = b->pending().time;
			return CompareFileTime(&aTime, &bTime) < 0;
		});
		_threadWorkerMutex.unlock();
	}

	boost::signals2::scoped_connection DeepMarket::Market::connect(const boost::signals2::signal<void(const CurrencyPairTrack*)>::slot_function_type  & slot) {
		return _marketChanged.connect(slot);
	}

	const std::vector<Currency*>& Market::currencies() const {
		return _currencies;
	}

	const Currency * Market::currency(const std::string & name) const {
		auto iterator = std::find_if(_currencies.begin(), _currencies.end(), [name](Currency* currency)->bool { return currency->name() == name; });
		if (iterator != _currencies.end()) return *iterator;
		return NULL;
	}

	void Market::_onMarketChanged(const CurrencyPairTrack* currencyPair) {
		_marketChanged(currencyPair);
	}

	void Market::step() {
		_threadWorkerMutex.lock();
		auto track = _sortedTracks[0];
		auto nextTime = track->pending().time;
		_time = nextTime;
		track->step();
		_onMarketChanged(track);

		if (_trackCount > 1) {
			auto index = 1;

			auto trackTime = track->pending().time;
			FILETIME t;

			while (index < _trackCount && CompareFileTime(&trackTime, &(t = _sortedTracks[index]->pending().time)) > 0) {
				_sortedTracks[index - 1] = _sortedTracks[index];
				_sortedTracks[index] = track;
				index++;
			}
		}
		_threadWorkerMutex.unlock();
	}

	ExchangeRate Market::operator[](const std::string& currencyPairName) const {
		auto tracks = std::find_if(_currencyPairs.begin(), _currencyPairs.end(), [&currencyPairName](CurrencyPairTrack* track)->bool { return currencyPairName == track->currencyPair().toString(); });
		auto track = tracks[0];
		auto priceInfo = track->current();
		return ExchangeRate(track->currencyPair(), priceInfo.time, priceInfo.ask, priceInfo.bid);
	}

	std::string Market::toString() const {
		std::vector<std::string> strings;
		auto buffer = new char[128];
		std::transform(_currencyPairs.begin(), _currencyPairs.end(), std::back_inserter(strings), [&buffer](const CurrencyPairTrack* track)->std::string {
			auto current = track->current();
			sprintf(
				buffer,
				"%6s %013.6f %013.6f",
				track->currencyPair().toString().c_str(),
				current.ask,
				current.bid
			);
			return std::string(buffer);
		});
		auto result = boost::algorithm::join(strings, "\n");
		delete buffer;
		return result;
	}

	FILETIME Market::time() const {
		return _time;
	}

	const std::vector<CurrencyPairTrack*>& Market::currencyPairs() const {
		return _currencyPairs;
	}

	int Market::trackCount() const {
		return _trackCount;
	}
	void Market::start() {
		_run = 1;
		_threadWorker = std::thread(&Market::_subRoutine, this);
	}
	void Market::stop() {
		_run = 0;
		_threadWorker.join();
	}

	void Market::_subRoutine() {
		while (_run) {
			step();
		}
	}
}