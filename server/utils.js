const BigNumber = require('bignumber.js');
const fs = require('fs');
const https = require('https');
const lnurl = require('lnurl');
const path = require('path');

const getRates = function(options) {
	if (typeof options !== 'object') {
		throw new Error('Invalid argument ("options"): Object expected');
	}
	if (!options.from) {
		throw new Error('Missing option: "from"');
	}
	if (!options.to) {
		throw new Error('Missing option: "to"');
	}
	return new Promise((resolve, reject) => {
		const done = function(error, result) {
			if (error) return reject(error);
			resolve(result);
		};
		const { from, to } = options;
		const uri = `https://api.coinbase.com/v2/exchange-rates?currency=${from}`;
		const req = https.request(uri, function(res) {
			let body = '';
			res.on('data', function(data) {
				body += data.toString();
			});
			res.on('end', function() {
				let rate;
				try {
					let json = JSON.parse(body);
					if (json.errors) {
						return done(new Error(JSON.stringify(json.errors)));
					}
					if (!json.data || !json.data.rates) {
						return done(new Error('Unexpected JSON response structure'));
					}
					if (!json.data.rates[to]) {
						return done(new Error(`Unsupported currency: "${to}"`));
					}
					rate = json.data.rates[to];
				} catch (error) {
					return done(error);
				}
				// Success!
				done(null, rate);
			});
		});
		req.on('error', error => {
			done(error);
		});
		req.end();
	});
};

const toSatoshis = function(amount, rate) {
	return (new BigNumber(amount))
			.dividedBy(rate)
			.times(1e8)
			.integerValue(BigNumber.ROUND_DOWN)
			.toNumber();
};

const toMilliSatoshis = function(amount, rate) {
	return toSatoshis(amount, rate) * 1000;
};

const loadConfig = (function() {

	const configExampleFilePath = path.join(__dirname, 'config-example.json');
	const configFilePath = path.join(__dirname, 'config.json');

	const generateConfigFile = function() {
		// Use the example config file as a starting point.
		let config = require(configExampleFilePath);
		// Generate an API key and add it to config.
		config.auth.apiKeys = [
			lnurl.generateApiKey({ encoding: 'base64' })
		];
		// Save to config.json.
		fs.writeFileSync(configFilePath, JSON.stringify(config, null, 4).replace(/ {4,4}/g,'\t'));
	};

	const configFileExists = function() {
		try {
			fs.statSync(configFilePath);
		} catch (error) {
			if (error.message.substr(0, 'ENOENT: no such file or directory'.length) !== 'ENOENT: no such file or directory') {
				console.error(error);
				process.exit(1);
			}
			return false;
		}
		return true;
	};

	return function() {
		if (!configFileExists()) {
			generateConfigFile();
		}
		return require(configFilePath);
	};
})();

module.exports = {
	loadConfig,
	getRates,
	toSatoshis,
	toMilliSatoshis,
};