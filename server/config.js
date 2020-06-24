/*
	Copyright (C) 2020 Samotari (Charles Hill, Carlos Garcia Ortiz)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Affero General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Affero General Public License for more details.

	You should have received a copy of the GNU Affero General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

const fs = require('fs');
const lnurl = require('lnurl');
const path = require('path');

const configFilePath = path.join(__dirname, 'config.json');

const generateConfigFile = function() {
	// Use the example config file as a starting point.
	let exampleConfig = getExampleConfig();
	saveConfigFile(exampleConfig);
};

const getExampleConfig = function() {
	return require(path.join(__dirname, 'config-example.json'));
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

const saveConfigFile = function(config) {
	// Save to config.json.
	fs.writeFileSync(configFilePath, JSON.stringify(config, null, 4).replace(/ {4,4}/g,'\t'));
};

const readConfigFile = function() {
	return JSON.parse(fs.readFileSync(configFilePath));
};

if (!configFileExists()) {
	generateConfigFile();
}

let config = readConfigFile();
let resave = false;

if (!config.auth.apiKeys || !(config.auth.apiKeys.length > 0)) {
	config.auth.apiKeys = [ lnurl.generateApiKey({ encoding: 'base64' }) ];
	resave = true;
}

if (typeof config.supportedFiatCurrencies === 'undefined') {
	config.supportedFiatCurrencies = getExampleConfig().supportedFiatCurrencies;
	resave = true;
}

if (resave) {
	saveConfigFile(config);
}

module.exports = config;
