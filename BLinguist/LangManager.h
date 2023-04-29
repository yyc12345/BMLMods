#pragma once

#include <vector>
#include <string>
#include <array>

namespace NSBLinguist::LangManager {

	using LangIdentifier = std::string;
	using UITrCollection = std::array<std::string, 71u>;
	using TutorialTrCollection = std::array<std::string, 10u>;

	bool IsLegalLangIdentifier(LangIdentifier& ident);
	bool DetectLanguages(LangIdentifier& out_ident);
	bool LoadTranslations(LangIdentifier& ident, UITrCollection& out_ui, TutorialTrCollection& out_tutorial);

}
