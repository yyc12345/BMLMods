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

	class OffsetCK2dEntity
	{
	public:
		OffsetCK2dEntity(CKContext* ctx, 
			const std::string& watching, const std::string& this_entity,
			const Vx2DVector& pos, const Vx2DVector& size
		);
		OffsetCK2dEntity(const OffsetCK2dEntity&) = delete;
		OffsetCK2dEntity& operator=(const OffsetCK2dEntity&) = delete;
		~OffsetCK2dEntity();

		void Process(void);
	private:
		CKContext* mCtx;
		CK2dEntity* mWatching;
		CK2dEntity* mCreated;
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
		std::vector<OffsetCK2dEntity*> mOffsetEntities;
		std::vector<LabelUI*> mUIList;
		// mTutorialList;

		CKContext* mCtx;
		int mUICounter;
	};

}
