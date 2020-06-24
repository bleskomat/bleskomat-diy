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

#include "lnurl.h"

namespace {

	std::string string_to_hex(const std::string& in) {
		std::stringstream ss;
		ss << std::hex << std::setfill('0');
		for (size_t i = 0; in.length() > i; ++i) {
			ss << std::setw(2) << static_cast<unsigned int>(static_cast<unsigned char>(in[i]));
		}
		return ss.str();
	}

	std::string bech32_encode(const std::string& hrp, const std::string& value) {
		std::vector<uint8_t> v = utilstrencodings::ParseHex(string_to_hex(value));
		std::vector<unsigned char> tmp = {};
		tmp.reserve(1 + 32 * 8 / 5);
		utilstrencodings::ConvertBits<8, 5, true>([&](unsigned char c) { tmp.push_back(c); }, v.begin(), v.end());
		std::string s(v.begin(), v.end());
		return bech32::encode(hrp, tmp);
	}

	std::string create_signature(const std::string &payload, const std::string &key) {
		std::string signature = "";
		// Convert std::string variables to char[]
		char keyChar[key.size() + 1];
		std::strcpy(keyChar, key.c_str());
		char payloadChar[payload.length() + 1];
		std::strcpy(payloadChar, payload.c_str());
		unsigned char hmacResult[32];
		mbedtls_md_context_t ctx;
		mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;
		const size_t payloadLength = std::strlen(payloadChar);
		const size_t keyLength = std::strlen(keyChar);
		mbedtls_md_init(&ctx);
		mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 1);
		mbedtls_md_hmac_starts(&ctx, (const unsigned char *) keyChar, keyLength);
		mbedtls_md_hmac_update(&ctx, (const unsigned char *) payloadChar, payloadLength);
		mbedtls_md_hmac_finish(&ctx, hmacResult);
		mbedtls_md_free(&ctx);
		for (int i = 0; i < sizeof(hmacResult); i++) {
			char str[3];
			sprintf(str, "%02x", (int)hmacResult[i]);
			signature.append(str);
		}
		return signature;
	}

	std::string generate_nonce_str() {
		std::ostringstream ss;
		ss << esp_random();
		return ss.str();
	}

	std::string url_encode(const std::string &value) {
		std::ostringstream escaped;
		escaped.fill('0');
		escaped << std::hex;
		for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
			std::string::value_type c = (*i);
			// Keep alphanumeric and other accepted characters intact
			if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
				escaped << c;
				continue;
			}
			// Any other characters are percent-encoded
			escaped << std::uppercase;
			escaped << '%' << std::setw(2) << int((unsigned char) c);
			escaped << std::nouppercase;
		}
		return escaped.str();
	}

	std::string construct_payload(const float &amount, const std::string &fiatCurrency, const std::string &apiKeyId) {
		std::string nonce = generate_nonce_str();
		std::string payload = "";
		std::ostringstream amountStringStream;
		amountStringStream << amount;
		std::string amountStr = amountStringStream.str();
		payload.append("id=");
		payload.append(url_encode(apiKeyId));
		payload.append("&n=");
		payload.append(url_encode(nonce));
		payload.append("&t=w");
		payload.append("&f=");
		payload.append(url_encode(fiatCurrency));
		payload.append("&pn=");
		payload.append(amountStr);
		payload.append("&px=");
		payload.append(amountStr);
		payload.append("&pd=");
		return payload;
	}
}

namespace lnurl {
	std::string create_signed_withdraw_request(
		const float &amount,
		const std::string &fiatCurrency,
		const std::string &apiKeyId,
		const std::string &apiKeySecret,
		const std::string &callbackUrl
	) {
		std::string payload = construct_payload(amount, fiatCurrency, apiKeyId);
		std::string signature = create_signature(payload, apiKeySecret);
		std::string hrp = "lnurl";
		std::string url = "";
		url.append(callbackUrl);
		url.append("?");
		url.append(payload);
		url.append("&s=");
		url.append(signature);
		return bech32_encode(hrp, url);
	}
}
