const amount = process.argv[2];

if (!amount) {
	console.error('Usage: SCRIPT <amount>\nExample: `npm run generate:signedLnurl "1.00"`');
	process.exit(1);
}

const crypto = require('crypto');
const querystring = require('querystring');

const createSignature = function(data, key) {
	if (typeof key === 'string' && isHex(key)) {
		key = Buffer.from(key, 'hex');
	}
	return crypto.createHmac('sha256', key).update(data).digest('hex');
};

const isHex = function(hex) {
	if (typeof key !== 'string') {
		throw new Error('Invalid argument ("hex"): String expected.');
	}
	return Buffer.from(hex, 'hex').toString('hex') === hex;
};

const generateNonce = function(numberOfBytes) {
	return crypto.randomBytes(numberOfBytes).toString('hex');
};

const config = require('../config');
const { id, key } = config.lnurl.auth.apiKeys[0];
const nonce = generateNonce(8);
const query = {
	id: id,
	n: nonce,
	// Note that tag and params are shortened to improve scannability of resulting QR codes.
	// See "Shorter Signed LNURLs" for more info:
	// https://github.com/chill117/lnurl-node#shorter-signed-lnurls
	t: 'withdrawRequest',
	f: config.fiatCurrency,
	pn: amount,
	px: amount,
	pd: '',
};
const payload = querystring.stringify(query);
query.s = createSignature(payload, key);
let baseUrl;
if (config.lnurl.url) {
	baseUrl = config.lnurl.url;
} else {
	const { host, port, protocol } = config.lnurl;
	baseUrl = `${protocol}://${host}:${port}`;
}
const { endpoint } = config.lnurl;
const signedUrl = `${baseUrl}${endpoint}?` + querystring.stringify(query);
// Print to standard output, but without a newline.
process.stdout.write(signedUrl);
