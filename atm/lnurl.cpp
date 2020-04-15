#include "lnurl.h"

namespace {
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
		const std::string &baseUrl
	) {
		std::string payload = construct_payload(amount, fiatCurrency, apiKeyId);
		std::string signature = create_signature(payload, apiKeySecret);
		std::string hrp = "lnurl";
		std::string url = "";
		url.append(baseUrl);
		url.append(payload);
		url.append("&s=");
		url.append(signature);
		return util::bech32_encode(hrp, url);
	}
}
