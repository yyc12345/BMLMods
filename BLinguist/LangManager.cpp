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
		{"Arabic", {1256 }},
	};

	/// <summary>
	/// Identifier, UI file name, Tutorial file name.
	/// Empty string mean that do not load.
	/// </summary>
	using LangLoadTuple = std::tuple<LangIdentifier, std::string, std::string>;
	static const std::vector<LangLoadTuple> g_LangLoadSymbols{
		{"English", "", "" },
		{"Chinese", "RCLANG_UI_ZH_CN", ""},
		{"Arabic", "RCLANG_UI_AR", ""},
	};

	static const std::vector<std::string> g_UITrEntryIdentifiers{
		"M_Main_But_5",
		"M_Main_But_4",
		"M_Main_But_3",
		"M_Main_But_2",
		"M_Main_But_1",
		"M_Start_But_Back",
		"M_Start_But_10",
		"M_Start_But_09",
		"M_Start_But_08",
		"M_Start_But_07",
		"M_Start_But_06",
		"M_Start_But_05",
		"M_Start_But_04",
		"M_Start_But_03",
		"M_Start_But_02",
		"M_Start_But_01",
		"M_Pause_But_Back",
		"M_Pause_But_4",
		"M_Pause_But_3",
		"M_Pause_But_2",
		"M_Pause_But_1",
		"M_YesNo_TextSprite",
		"M_YesNo_But_Yes",
		"M_YesNo_But_No",
		"M_Dead_But_2",
		"M_Dead_But_1",
		"M_End_But_5",
		"M_End_But_4",
		"M_End_But_3",
		"M_End_But_2",
		"M_End_But_1",
		"M_Credits_But_Back",
		"M_Highscore_But_Back",
		"M_Options_But_Back",
		"M_HighEntry_But_1",
		"M_HighEntry_Title",
		"M_Highscore_Title",
		"M_Options_But_1",
		"M_Options_But_2",
		"M_Options_But_3",
		"M_Options_Title",
		"M_Opt_Graph_Title",
		"M_Opt_Gra_ResField2",
		"M_Opt_Gra_Back",
		"M_Opt_Gra_SynchYes",
		"M_Opt_Graph_SynchField2",
		"M_Opt_Gra_SynchNo",
		"M_Opt_Gra_CloudsYes",
		"M_Opt_Gra_CloudsNo",
		"M_Opt_Gra_CloudsField2",
		"M_Opt_Keys_Title",
		"M_Opt_Keys_Inv_Yes",
		"M_Opt_Keys_Inv_No",
		"M_Opt_Keys_Inv_Field2",
		"M_Opt_Keys_But_Back",
		"M_Opt_Keys_Key6",
		"M_Opt_Keys_Key4",
		"M_Opt_Keys_Key3",
		"M_Opt_Keys_Key2",
		"M_Opt_Keys_Key1",
		"M_Opt_Keys_Key5",
		"M_Opt_Sound_Title",
		"M_Opt_Sound_But_Back",
		"M_Opt_Sound_VolField2",
		"M_Score_Text4",
		"M_Score_Text3",
		"M_Score_Text2",
		"M_Score_Text1",
		"M_Start_But_12",
		"M_Start_But_11"
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
		out_ui.clear();
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

			for (const auto& ui : g_UITrEntryIdentifiers) {
				std::string trdata;
				if (!std::getline(ss, trdata, '\n')) return false;

				out_ui.emplace_back(std::make_pair(ui, std::move(trdata)));
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
