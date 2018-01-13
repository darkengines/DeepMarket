#include "Market.hpp"
#include <boost/filesystem.hpp>
#include <functional>
#include <boost/algorithm/string/join.hpp>

namespace DeepMarket {

	Market::Market(const std::string & dataDirPath) {
		boost::filesystem::directory_iterator directoryIterator(dataDirPath);
		boost::filesystem::directory_iterator iteratorEnd;

		while (directoryIterator != iteratorEnd) {
			if (boost::filesystem::is_regular_file(directoryIterator->path())) {
				boost::filesystem::path path = directoryIterator->path();
				auto name = path.filename().replace_extension("").string();
				auto filePath = path.string();
				auto currencyPair = new CurrencyPair(name, filePath);
				_currencyPairs.push_back(currencyPair);
			}
			directoryIterator++;
		}
		_trackCount = _currencyPairs.size();
		_sortedTracks = new CurrencyPair*[_trackCount];
		memcpy(_sortedTracks, _currencyPairs.data(), _trackCount * sizeof(CurrencyPair*));
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
		std::sort(_sortedTracks, _sortedTracks + _trackCount, [](const CurrencyPair* a, const CurrencyPair* b)->bool {
			auto aTime = a->pending().time;
			auto bTime = b->pending().time;
			return CompareFileTime(&aTime, &bTime) < 0;
		});
		_threadWorkerMutex.unlock();
	}

	boost::signals2::scoped_connection DeepMarket::Market::connect(const boost::signals2::signal<void(const price_info&)>::slot_function_type  & slot) {
		return _marketChanged.connect(slot);
	}

	void Market::_onMarketChanged(const price_info & priceInfo) {
		_marketChanged(priceInfo);
	}

	void Market::step() {
		_threadWorkerMutex.lock();
		auto track = _sortedTracks[0];
		auto nextTime = track->pending().time;
		_time = nextTime;
		track->step();
		_onMarketChanged(track->current());

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

	CurrencyPair * Market::operator[](const std::string & pairName) const {
		auto tracks = std::find_if(_currencyPairs.begin(), _currencyPairs.end(), [&pairName](CurrencyPair* track)->bool { return pairName == track->name(); });
		return tracks[0];
	}

	std::string Market::toString() const {
		std::vector<std::string> strings;
		auto buffer = new char[128];
		std::transform(_currencyPairs.begin(), _currencyPairs.end(), std::back_inserter(strings), [&buffer](const CurrencyPair* track)->std::string {
			auto current = track->current();
			sprintf(
				buffer,
				"%6s %013.6f %013.6f",
				track->name().c_str(),
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

	const std::vector<CurrencyPair*>& Market::currencyPairs() const {
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