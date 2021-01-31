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

const amount = process.argv[2];
if (!amount) {
	console.error('Usage: SCRIPT <amount>\nExample: `npm run generate:signedLnurl "1.00"`');
	process.exit(1);
}
const lnurl = require('lnurl');
const config = require('../config');
const apiKey = config.lnurl.auth.apiKeys[0];
let baseUrl = config.lnurl.url;
if (config.lnurl.endpoint) {
	baseUrl += config.lnurl.endpoint;
}
const options = {
	baseUrl,
	encode: false,
	shorten: false,
};
if (config.fiatCurrency) {
	params.f = config.fiatCurrency;
}
const tag = 'withdrawRequest';
let params = {
	minWithdrawable: amount,
	maxWithdrawable: amount,
	defaultDescription: '',
};
const signedUrl = lnurl.createSignedUrl(apiKey, tag, params, options);
// Print to standard output, but without a newline.
process.stdout.write(signedUrl);
