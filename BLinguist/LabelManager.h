#pragma once

#include <BML/BMLAll.h>
#include "LangManager.h"
#include <string>

namespace NSBLinguist::LabelManager {

	class LabelUI {
	public:
		LabelUI(CKContext* ctx,
			const std::string& watching, const std::string& text,
			const std::string& fontname, const int fontsize);
		LabelUI(const LabelUI&) = delete;
		LabelUI& operator=(const LabelUI&) = delete;
		~LabelUI();

		void Process(void);
	private:
		void InitBGuiText(const std::string& text, const std::string& fontname, const int fontsize);

		BGui::Text mOper;
		CKContext* mCtx;
		CK2dEntity* mWatchingEntity;
	};


	class LabelsCollection {
	public:
		LabelsCollection(CKContext* ctx,
			const LangManager::UITrCollection& ui, const LangManager::TutorialTrCollection& tutorial,
			const std::string& fontname, const int fontsize);
		~LabelsCollection();

		void Process(void);
	private:
		std::vector<LabelUI*> mUIList;
		// mTutorialList;

		CKContext* mCtx;
		int mUICounter;
	};

}
