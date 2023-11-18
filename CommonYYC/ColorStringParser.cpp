#include "YYCHelper.h"
#include <cinttypes>
#include <regex>
#include <string>

namespace YYCHelper::ColorStringParser {

	static bool ParseFactor(const std::string& num, uint8_t& factor, int base) {
		try {
			unsigned long long ret = std::stoull(num, nullptr, base);
			if (ret > 255) return false;

			factor = static_cast<uint8_t>(ret);
			return true;
		} catch (const std::invalid_argument&) {
			return false;
		} catch (const std::out_of_range&) {
			return false;
		}
	}

	static bool ParseDecRGBColorString(std::string& strl, VxColor& result) {
		// try check rrr,ggg,bbb style. build regex
		static std::regex re("([0-9]{1,3}) *, *([0-9]{1,3}) *, *([0-9]{1,3})", std::regex_constants::ECMAScript);
		std::smatch base_match;

		if (std::regex_search(strl, base_match, re)) {
			if (base_match.size() != 4) return false;

			uint8_t r, g, b;
			if (!ParseFactor(base_match[1].str(), r, 10)) return false;
			if (!ParseFactor(base_match[2].str(), g, 10)) return false;
			if (!ParseFactor(base_match[3].str(), b, 10)) return false;

			result.Set(r, g, b, 255);
			return true;
		} else {
			return false;
		}
	}

	static bool ParseHashTagHexColorString(std::string& strl, VxColor& result) {
		// try check #rrggbb style. build regex
		static std::regex re("#([a-fA-F0-9]{2})([a-fA-F0-9]{2})([a-fA-F0-9]{2})", std::regex_constants::ECMAScript);
		std::smatch base_match;

		if (std::regex_search(strl, base_match, re)) {
			if (base_match.size() != 4) return false;
			
			uint8_t r, g, b;
			if (!ParseFactor(base_match[1].str(), r, 16)) return false;
			if (!ParseFactor(base_match[2].str(), g, 16)) return false;
			if (!ParseFactor(base_match[3].str(), b, 16)) return false;

			result.Set(r, g, b, 255);
			return true;
		} else {
			return false;
		}
	}

	bool ParseColorString(const char* colstr, VxColor& result) {
		// prepare default result (white)
		static VxColor default_result(255, 255, 255, 255);

		// check null. if not, parse it to std::string for regex.
		if (colstr == nullptr) {
			result = default_result;
			return false;
		}
		std::string stdcol(colstr);

		// try parse with 2 style
		if (ParseDecRGBColorString(stdcol, result)) return true;
		if (ParseHashTagHexColorString(stdcol, result)) return true;

		// if no match, return default
		result = default_result;
		return false;
	}

}
