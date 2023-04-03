#pragma once

#include <BML/BMLAll.h>
#include "LangManager.h"
#include <string>

namespace NSBLinguist::LabelManager {

	class BBTextDisplay {
	public:
		BBTextDisplay(CKContext* ctx);
		BBTextDisplay(const BBTextDisplay&) = delete;
		BBTextDisplay& operator=(const BBTextDisplay&) = delete;
		~BBTextDisplay();

		void SetVisible(bool is_visible);
		void SetText(CKSTRING strl);
		void SetFont(CKSTRING fontname, int fontsize);
		void SetPosition(const Vx2DVector& pos);
	private:
		CKContext* ctx;
		CKSpriteText* tt;
	};

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
		BBTextDisplay mOper;
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
