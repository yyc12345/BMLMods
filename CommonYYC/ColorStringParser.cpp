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

	static bool ParseDecRGBColorString(const std::string& strl, VxColor& result) {
		// try check rrr,ggg,bbb style or rrr,ggg,bbb,aaa style. build regex
		// ?: is non-captured group.
		static std::regex re("([0-9]{1,3}) *, *([0-9]{1,3}) *, *([0-9]{1,3})(?: *, *([0-9]{1,3}))?", std::regex_constants::ECMAScript);
		std::smatch base_match;

		if (std::regex_search(strl, base_match, re)) {
			if (base_match.size() != 5) return false;
			
			uint8_t r, g, b, a;
			if (!ParseFactor(base_match[1].str(), r, 10)) return false;
			if (!ParseFactor(base_match[2].str(), g, 10)) return false;
			if (!ParseFactor(base_match[3].str(), b, 10)) return false;
			if (base_match[4].matched) {
				if (!ParseFactor(base_match[4].str(), a, 10)) return false;
			} else {
				a = 255;
			}

			result.Set(r, g, b, a);
			return true;
		} else {
			return false;
		}
	}

	static bool ParseHashTagHexColorString(const std::string& strl, VxColor& result) {
		// try check #rrggbb style or #rrggbbaa style. build regex
		static std::regex re("#([a-fA-F0-9]{2})([a-fA-F0-9]{2})([a-fA-F0-9]{2})([a-fA-F0-9]{2})?", std::regex_constants::ECMAScript);
		std::smatch base_match;

		if (std::regex_search(strl, base_match, re)) {
			if (base_match.size() != 5) return false;
			
			uint8_t r, g, b, a;
			if (!ParseFactor(base_match[1].str(), r, 16)) return false;
			if (!ParseFactor(base_match[2].str(), g, 16)) return false;
			if (!ParseFactor(base_match[3].str(), b, 16)) return false;
			if (base_match[4].matched) {
				if (!ParseFactor(base_match[4].str(), a, 16)) return false;
			} else {
				a = 255;
			}

			result.Set(r, g, b, a);
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
