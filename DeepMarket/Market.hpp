#pragma once
#include <string>
#include <vector>
#include <Windows.h>
#include <list>
#include <boost/filesystem.hpp>
#include <thread>
#include "CurrencyPairTrack.hpp"
#include "CurrencyPair.hpp"
#include "Currency.hpp"
#include "ExchangeRate.hpp"
#include <boost/signals2.hpp>
#include <mutex>

namespace DeepMarket {
	class Market {
	public:
		Market(const std::string& dataDirPath);
		Market(const Market& market);
		~Market();
		void seek(SYSTEMTIME time);
		void step();
		ExchangeRate operator[](const std::string& pairName) const;
		std::string toString() const;
		FILETIME time() const;
		const std::vector<CurrencyPairTrack*>& currencyPairs() const;
		int trackCount() const;
		void start();
		void stop();
		boost::signals2::scoped_connection connect(const boost::signals2::signal<void(const CurrencyPairTrack*)>::slot_function_type & slot);
		const std::vector<Currency*>& currencies() const;
		const Currency* currency(const std::string& name) const;
	protected:
		std::vector<CurrencyPairTrack*> _currencyPairs;
		void _onMarketChanged(const CurrencyPairTrack* currencyPair);
		boost::signals2::signal<void(const CurrencyPairTrack*)> _marketChanged;
		CurrencyPairTrack** _sortedTracks = 0;
		FILETIME _time;
		int _trackCount = 0;
		std::thread _threadWorker;
		void _subRoutine();
		bool _run = false;
		std::mutex _threadWorkerMutex;
		std::vector<Currency*> _currencies;
	};
}