#include "LangManager.h"
#include "DllMain.h"
#include <Windows.h>
#include <sstream>

namespace NSBLinguist::LangManager {

	using LangDetectPair = std::pair<LangIdentifier, std::vector<UINT>>;
	static const LangIdentifier g_DefaultLang = "English";
	static const std::vector<LangDetectPair> g_LangCodePageAssoc{
		{"English", {437, }},
		{"Chinese", {936, 20936 }},
		{"Arabic", {720, 1256 }},
		{"Japanese", {932 }},
	};

	/// <summary>
	/// Identifier, UI file name, Tutorial file name.
	/// Empty string mean that do not load.
	/// </summary>
	using LangLoadTuple = std::tuple<LangIdentifier, std::string, std::string>;
	static const std::vector<LangLoadTuple> g_LangLoadSymbols{
		{"English", "RCLANG_UI_EN", "RCLANG_TUTORIAL_EN" },
		{"Chinese", "RCLANG_UI_ZH_CN", "RCLANG_TUTORIAL_ZH_CN"},
		{"Arabic", "RCLANG_UI_AR", "RCLANG_TUTORIAL_AR"},
		{"Japanese", "RCLANG_UI_JA", "RCLANG_TUTORIAL_JA"},
	};

	static bool GetLangLoadSymbols(LangIdentifier& ident, std::string& out_ui, std::string& out_tutorial) {
		for (const auto& [lang_ident, lang_ui, lang_tutorial] : g_LangLoadSymbols) {
			if (lang_ident == ident) {
				out_ui = lang_ui;
				out_tutorial = lang_tutorial;
				return true;
			}
		}

		out_ui.clear();
		out_tutorial.clear();
		return false;
	}

	static bool LoadWinRes(std::string& symbol, std::stringstream& out_ss) {
		// clear first
		out_ss.str("");
		out_ss.clear();
		
		// find resource
		HRSRC hRsrc = FindResource(NSBLinguist::hSelfHandle, symbol.c_str(), RT_RCDATA);
		if (hRsrc == nullptr) return false;
		// get resources size
		DWORD dwSize = SizeofResource(NSBLinguist::hSelfHandle, hRsrc);
		if (dwSize == 0u) return false;
		// load resources
		HGLOBAL hGlobal = LoadResource(NSBLinguist::hSelfHandle, hRsrc);
		if (hGlobal == nullptr) return false;
		// lock resources
		LPVOID pBuffer = LockResource(hGlobal);
		if (pBuffer == nullptr) return false;

		// write into stream
		out_ss.write(reinterpret_cast<char*>(pBuffer), dwSize);
		return true;
	}

	bool IsLegalLangIdentifier(LangIdentifier& ident) {
		for (const auto& lang : g_LangCodePageAssoc) {
			if (lang.first == ident) return true;
		}

		return false;
	}

	bool DetectLanguages(LangIdentifier& out_ident) {
		UINT cp = GetACP();

		for (const auto& lang : g_LangCodePageAssoc) {
			for (const auto& langcp : lang.second) {
				if (langcp == cp) {
					out_ident = lang.first;
					return true;
				}
			}
		}

		out_ident = g_DefaultLang;
		return false;
	}

	bool LoadTranslations(LangIdentifier& ident, UITrCollection& out_ui, TutorialTrCollection& out_tutorial) {
		// clear result first
		for (auto& subt : out_ui) {
			subt.clear();
		}
		for (auto& subt : out_tutorial) {
			subt.clear();
		}

		// try get symbols
		std::string symbol_ui, symbol_tutorial;
		if (!GetLangLoadSymbols(ident, symbol_ui, symbol_tutorial)) return false;

		// load translations
		std::stringstream ss;
		// load ui translation
		if (!symbol_ui.empty()) {
			if (!LoadWinRes(symbol_ui, ss)) return false;

			for (auto& trdata : out_ui) {
				if (!std::getline(ss, trdata, '\n')) return false;
			}
		}

		// load tutorial translation
		if (!symbol_tutorial.empty()) {
			if (!LoadWinRes(symbol_tutorial, ss)) return false;

			for (auto& tutorial : out_tutorial) {
				if (!std::getline(ss, tutorial, '*')) return false;
			}
		}

		return true;
	}
}
