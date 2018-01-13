#pragma once
#include <string>
#include <vector>
#include <Windows.h>
#include <list>
#include <boost/filesystem.hpp>
#include <thread>
#include "CurrencyPair.hpp"
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
		CurrencyPair* operator[](const std::string& pairName) const;
		std::string toString() const;
		FILETIME time() const;
		const std::vector<CurrencyPair*>& currencyPairs() const;
		int trackCount() const;
		void start();
		void stop();
		boost::signals2::scoped_connection connect(const boost::signals2::signal<void(const price_info&)>::slot_function_type & slot);
	protected:
		std::vector<CurrencyPair*> _currencyPairs;
		void _onMarketChanged(const price_info&);
		boost::signals2::signal<void(const price_info&)> _marketChanged;
		CurrencyPair** _sortedTracks = 0;
		FILETIME _time;
		int _trackCount = 0;
		std::thread _threadWorker;
		void _subRoutine();
		bool _run = false;
		std::mutex _threadWorkerMutex;
	};
}