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

const lnurl = require('lnurl');
const { HttpError } = lnurl.Server;
const { getRates, toSatoshis, toMilliSatoshis } = require('./utils');

/*
	For a list of possible options, see:
	https://github.com/chill117/lnurl-node#options-for-createserver-method
*/
const config = require('./config');
const server = lnurl.createServer(config.lnurl);

server.bindToHook('middleware:signedLnurl:afterCheckSignature', function(req, res, next) {
	if (req.query.f) {
		// Convert fiat amounts to satoshis.
		const fiatSymbol = req.query.f;
		try {
			return getRates({
				from: 'BTC',
				to: fiatSymbol,
			}).then(rate => {
				const { tag } = req.query;
				switch (tag) {
					case 'withdrawRequest':
						const { minWithdrawable, maxWithdrawable } = req.query;
						req.query.minWithdrawable = toMilliSatoshis(minWithdrawable, rate);
						req.query.maxWithdrawable = toMilliSatoshis(maxWithdrawable, rate);
						break;
					default:
						throw new HttpError(`Unsupported tag: "${tag}"`);
				}
				next();
			}).catch(next);
		} catch (error) {
			return next(error);
		}
	}
	next();
});

process.on('uncaughtException', (error, origin) => {
	console.error(error);
});

process.on('beforeExit', (code) => {
	try {
		server.close();
	} catch (error) {
		console.error(error);
	}
	process.exit(code);
});
