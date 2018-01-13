const fetch = require('node-fetch');
const url = require('url');
const zlib = require('zlib');
const fs = require('fs');
var HttpProxyAgent = require('http-proxy-agent');
var mkdirp = require('mkdirp');
const Throttle = require('promise-parallel-throttle');
const timeout = ms => new Promise(res => setTimeout(res, ms));

const seq = function(jobs) {
	
}

const host = 'http://www.histdata.com';

 const getPairs = async () => {
	try {
		const regex = /\/download-free-forex-historical-data\/\?\/ascii\/tick-data-quotes\/([A-Z]{6})/g;
		const response = await fetch(url.resolve(host, '/download-free-forex-data/?/ascii/tick-data-quotes'), {
			timeout: 0
		});
		if (response.status != 200) throw new Error(`status ${response.status}`);
		const page = await response.text();
		let match = regex.exec(page);
		pairs = [];
		while (match) {
			pairs.push(match[1]);
			match = regex.exec(page);
		}
		return pairs;
	} catch (e) {
		console.log('getPairs failed, trying again...', e);
		return await getPairs();
	}
};

const getPairYears = async (pair) => {
	try {
		const pairUrl = `/download-free-forex-historical-data/?/ascii/tick-data-quotes/${pair}`;
		const regex = new RegExp(`\/download-free-forex-historical-data\/\\?\/ascii\/tick-data-quotes\/${pair}\/([0-9]{4})`, 'gi');
		const response = await fetch(url.resolve(host, pairUrl), {
			timeout: 0
		});
		if (response.status != 200) throw new Error(`status ${response.status}`);
		const page = await response.text();
		let match = regex.exec(page);
		const years = [];
		while (match) {
			years.push(match[1]);
			match = regex.exec(page);
		}
		return years;
	} catch (e) {
		console.log(`getPairYears(${pair}) failed, trying again...`, e);
		return await getPairYears(pair);
	}
};

const getPairYearMonths = async(pair, year) => {
	try {
		const pairYearUrl = `/download-free-forex-historical-data/?/ascii/tick-data-quotes/${pair}/${year}`;
		const regex = new RegExp(`\/download-free-forex-historical-data\/\\?\/ascii\/tick-data-quotes\/${pair}\/${year}\/([0-9]{1,2})`, 'gi');
		const response = await fetch(url.resolve(host, pairYearUrl), {
			timeout: 0,
		});
		if (response.status != 200) throw new Error(`status ${response.status}`);
		const page = await response.text();
		let match = regex.exec(page);
		const months = [];
		while (match) {
			months.push(match[1]);
			match = regex.exec(page);
		}
		return months;
	} catch (e) {
		console.log(`getPairYearMonths(${pair}, ${year}) failed, trying again...`, e);
		return await getPairYearMonths(pair, year);
	}
};

const getPairYearMonthToken = async(pair, year, month) => {
	try {
		const pairYearUrl = `/download-free-forex-historical-data/?/ascii/tick-data-quotes/${pair}/${year}/${month}`;
		const regex = new RegExp('value="(.{32})"', 'gi');
		const response = await fetch(url.resolve(host, pairYearUrl), {
			timeout: 0,
		});
		if (response.status != 200) throw new Error(`status ${response.status}`);
		const page = await response.text();
		let match = regex.exec(page);
		const token = match && match[1];
		console.log(token);
		return token;
	} catch (e) {
		console.log(`getPairYearMonthToken(${pair}, ${year}, ${month}) failed, trying again...`, e);
		return await getPairYearMonthToken(pair, year, month);
	}
};

const download = async function(pair, year, month, token) {
	return new Promise(async (resolve, error) => {
		if ((month+'').length < 2) month = '0'+month
		const getUrl = 'get.php';
		const response = await fetch(url.resolve(host, getUrl), {
			method: 'POST',
			headers: {
				Referer: `http://www.histdata.com/download-free-forex-historical-data/?/ascii/tick-data-quotes/${pair}/${year}/${month}`,
				'Content-Type': 'application/x-www-form-urlencoded',
				'accept-encoding': 'gzip, deflate',
				'Accept': '*/*'
			},
			timeout: 0,
			body: `tk=${token}&date=${year}&datemonth=${year}${month}&platform=ascii&timeframe=T&fxpair=${pair}`
		});
		const path = `./data/${pair}`
		mkdirp(path);
		const file = fs.createWriteStream(`${path}/${year}-${month}.zip`);
		response.body.pipe(file);
		response.body.on('end', () => resolve());
	});
};

const data = {};

//download('EURUSD', 2016, 6).then(buffer => {});

getPairs().then(pairs => {
	pairs.forEach(pair => data[pair] = {});
	const jobs = pairs.map(pair => () => {
		return getPairYears(pair).then(pairYears => {
			pairYears.forEach(pairYear => data[pair][pairYear] = {});
			const jobs = pairYears.map(pairYear => () => {
				return getPairYearMonths(pair, pairYear).then(pairYearMonths => {
					const jobs = pairYearMonths.map(pairYearMonth => () => {
						return getPairYearMonthToken(pair, pairYear, pairYearMonth).then(token => download(pair, pairYear, pairYearMonth, token))
					});
					return Throttle.all(jobs)
				});
			});
			return Throttle.all(jobs);
		});
	});
	return Throttle.all(jobs);
}).then(() => console.log(data));