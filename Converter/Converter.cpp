#include <boost/filesystem.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/sort/sort.hpp>
#include <iostream>
#include <zip.h>
#include <mutex>
#include <numeric>
#include "read_file_result.h"
#include "Converter.h"

using namespace boost::filesystem;
using namespace boost::iostreams;
using namespace std;

void Converter::convert(const char * inputDir, const char * outputFile, bool(*predicate)(int, int), int numThreads) {
	path dataPath(inputDir);
	directory_iterator directoryIteratorEnd;
	directory_iterator directoryIterator(dataPath);

	std::mutex lock;
	vector<read_file_result_t> readFileResults;

	boost::asio::io_service ioService;
	boost::thread_group threadpool;
	boost::asio::io_service::work work(ioService);

	while (numThreads--) {
		threadpool.create_thread(
			boost::bind(&boost::asio::io_service::run, &ioService)
		);
	}

	int order = 0;
	while (directoryIterator != directoryIteratorEnd) {
		path zipPath = directoryIterator->path();

		int year, month;
		sscanf(zipPath.filename().string().c_str(), "%4d-%2d.zip", &year, &month);
		if (!predicate || predicate(year, month)) {
			std::function<void(int)> method = [&, zipPath](int order)->void {
				char* path = new char[zipPath.string().length() + 1];
				path[zipPath.string().length()] = '\0';
				wcstombs(path, zipPath.c_str(), zipPath.string().length());
				DeepMarket::price_info* priceInfos;
				std::size_t priceInfosCount;
				auto error = readFile(path, &priceInfos, &priceInfosCount);
				if (!error) {
					read_file_result_t result;
					result.order = order;
					result.price_infos = priceInfos;
					result.price_infos_count = priceInfosCount;

					lock.lock();
					readFileResults.push_back(result);
					lock.unlock();
				}
				delete path;
			};
			ioService.post(boost::bind(method, order));
			order++;
		}
		directoryIterator++;
	}

	ioService.stop();
	threadpool.join_all();

	std::sort(readFileResults.begin(), readFileResults.end(), [](const read_file_result_t& a, const read_file_result_t& b)->bool { return a.order < b.order; });
	auto size = std::accumulate(readFileResults.begin(), readFileResults.end(), 0, [](int sum, const read_file_result_t& result)->std::size_t { return sum + result.price_infos_count; });

	auto data = new DeepMarket::price_info[size];
	auto dataIndex = data;
	int arrayIndex = 0;
	for (auto &readFileResult : readFileResults) {
		auto priceInfoArraySize = readFileResult.price_infos_count;
		auto arraySize = priceInfoArraySize * sizeof(DeepMarket::price_info);
		memcpy(dataIndex, readFileResult.price_infos, arraySize);
		delete readFileResult.price_infos;
		dataIndex += priceInfoArraySize;
		arrayIndex++;
	}

	std::ofstream outputStream(outputFile, std::ios::out | std::ios::binary);
	outputStream.write((const char*)data, size * sizeof(DeepMarket::price_info));
	outputStream.close();

	delete data;
}

int Converter::readFile(const char * inputFile, DeepMarket::price_info ** outPriceInfos, std::size_t * outPriceInfosCount) {
	vector<DeepMarket::price_info> priceInfos;
	int zipError;
	auto zip = zip_open(inputFile, ZIP_RDONLY, &zipError);
	zip_stat_t zipStat;
	auto zipStatError = zip_stat_index(zip, 0, ZIP_STAT_SIZE, &zipStat);
	if (!zipStatError) {
		auto zipFile = zip_fopen_index(zip, 0, ZIP_FL_UNCHANGED);
		auto buffer = new char[zipStat.size];
		zip_fread(zipFile, buffer, zipStat.size);
		zip_fclose(zipFile);
		auto start = buffer;
		auto end = buffer;
		auto lineBuffer = new char[1024];
		auto endOfFile = buffer + zipStat.size;
		while ((end = (char*)memchr(buffer, '\n', endOfFile - buffer))) {
			memcpy(lineBuffer, buffer, end - buffer);
			lineBuffer[end - buffer] = '\0';

			float buyPrice, sellPrice;
			int year, month, day, hour, minute, seconds, milliseconds;
			sscanf_s(lineBuffer, "%4d%2d%2d %2d%2d%2d%3d,%f,%f", &year, &month, &day, &hour, &minute, &seconds, &milliseconds, &buyPrice, &sellPrice);

			SYSTEMTIME sysDateTime;
			sysDateTime.wYear = year;
			sysDateTime.wMonth = month;
			sysDateTime.wDay = day;
			sysDateTime.wHour = hour;
			sysDateTime.wMinute = minute;
			sysDateTime.wSecond = seconds;
			sysDateTime.wMilliseconds = milliseconds;

			DeepMarket::price_info currentPriceInfo;
			SystemTimeToFileTime(&sysDateTime, &currentPriceInfo.time);
			currentPriceInfo.ask = buyPrice;
			currentPriceInfo.bid = sellPrice;
			priceInfos.push_back(currentPriceInfo);

			buffer = end + 1;
		}
		delete lineBuffer;
		delete start;

		*outPriceInfosCount = priceInfos.size();
		*outPriceInfos = new DeepMarket::price_info[*outPriceInfosCount];
		memcpy(*outPriceInfos, priceInfos.data(), *outPriceInfosCount * sizeof(DeepMarket::price_info));
		return 0;
	}
	zip_close(zip);
	return -1;
}

void Converter::convertPairs(const char * inputDir, const char * outputDir, bool(*predicate)(int, int)) {
	path inputDirPath(inputDir);
	path outputDirPath(outputDir);
	directory_iterator directoryIteratorEnd;
	directory_iterator directoryIterator(inputDirPath);

	while (directoryIterator != directoryIteratorEnd) {
		path pairInputDirPath = directoryIterator->path();
		if (boost::filesystem::is_directory(pairInputDirPath)) {
			auto pairOutputFilePath = outputDir / pairInputDirPath.filename().replace_extension("dat");
			convert(pairInputDirPath.string().c_str(), pairOutputFilePath.string().c_str(), predicate);
		}
		directoryIterator++;
	}
}
