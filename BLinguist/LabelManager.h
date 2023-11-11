#pragma once

#include <YYCHelper.h>
#include "LangManager.h"
#include <string>

namespace NSBLinguist::LabelManager {

	class BBTextDisplay {
	public:
		BBTextDisplay(CKContext* ctx);
		BBTextDisplay(const BBTextDisplay&) = delete;
		BBTextDisplay& operator=(const BBTextDisplay&) = delete;
		~BBTextDisplay();

		void SetAlignment(CKSPRITETEXT_ALIGNMENT alignment);
		void SetVisible(bool is_visible);
		void SetText(const std::string& text);
		void SetFont(CKSTRING fontname, int fontsize);
		void SetColor(CKDWORD colARGB);
		void SetPosition(const Vx2DVector& pos);
		void SetSize(const Vx2DVector& size);
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
			const std::string& fontname, const int fontsize, CKDWORD fontcolor);
		LabelUI(const LabelUI&) = delete;
		LabelUI& operator=(const LabelUI&) = delete;
		~LabelUI();

		void Process(void);
	private:
		BBTextDisplay mOper;
		CKContext* mCtx;
		CK2dEntity* mWatchingEntity;
		int mFontSize;
	};

	class LabelTutorial
	{
	public:
		LabelTutorial(CKContext* ctx, const std::string& text,
			const std::string& fontname, const int fontsize, CKDWORD fontcolor);
		LabelTutorial(const LabelTutorial&) = delete;
		LabelTutorial& operator=(const LabelTutorial&) = delete;
		~LabelTutorial();

		void SetVisible(bool is_visible);
		void Process(const Vx2DVector& tut_pos);
	private:
		std::vector<BBTextDisplay*> mLines;
		CKContext* mCtx;
		int mFontSize;
	};


	class LabelsCollection {
	public:
		LabelsCollection(CKContext* ctx,
			const LangManager::UITrCollection& ui, const LangManager::TutorialTrCollection& tutorial,
			const std::string& fontname, const int fontsize, CKDWORD fontcolor);
		~LabelsCollection();

		void Process(void);
	private:
		std::vector<OffsetCK2dEntity*> mOffsetEntities;
		std::vector<LabelUI*> mUIList;
		std::vector<LabelTutorial*> mTutorialList;

		bool GetTutorialObjects(CKParameterLocal** ppindex, CK2dEntity** pTutInterface);

		CKContext* mCtx;
		int mUICounter;
	};

}
