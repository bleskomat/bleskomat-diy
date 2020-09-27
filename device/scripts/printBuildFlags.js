/*
	Copyright (C) 2020 Samotari (Charles Hill, Carlos Garcia Ortiz)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

const printBuildFlags = function(buildFlags) {
	const output = Object.keys(buildFlags).map(function(name) {
		const value = buildFlags[name];
		// Use single quotes around whole build flag definition + double-quotes around value:
		return `'-D ${name}="${value}"'`;
	}).join('\n');
	process.stdout.write(output);
};

const config = require('../../server/config');
const apiKey = config.lnurl.auth.apiKeys[0] || null;
const { endpoint } = config.lnurl;

if (!apiKey) {
	console.error('Must configure at least one API key; see "auth.apiKeys" in server/config.json');
	process.exit(1);
}

let baseUrl;
if (config.lnurl.url) {
	baseUrl = config.lnurl.url;
} else {
	const { host, port, protocol } = config.lnurl;
	baseUrl = `${protocol}://${host}:${port}`;
}

let buildFlags = {
	API_KEY_ID: apiKey.id,
	API_KEY_SECRET: apiKey.key,
	CALLBACK_URL: `${baseUrl}${endpoint}`,
	FIAT_CURRENCY: config.fiatCurrency,
};

printBuildFlags(buildFlags);
