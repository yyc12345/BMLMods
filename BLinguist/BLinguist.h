#pragma once

#include <YYCHelper.h>
#include "../FontCraft/main.h"
#include "LangManager.h"
#include "LabelManager.h"

struct BLinguistSettings {
	bool mEnabled;
	NSBLinguist::LangManager::UITrCollection mUITr;
	NSBLinguist::LangManager::TutorialTrCollection mTutorialTr;

	std::string FontName;
	int FontSize;
	CKDWORD FontColor;
};

class BLinguist : public IMod {

	// solve the problem that get links from CKBehaviorIO
#if defined(YYCMOD_BMLP_USED)
	friend class CKBehaviorIO;
#endif

public:
	BLinguist(IBML* bml) :
		IMod(bml), mLaunchSettings(), mLabelsCollection(nullptr) {
	}

	virtual YYCBML_CKSTRING GetID() override { return "BLinguist"; }
	virtual YYCBML_CKSTRING GetVersion() override { return YYCMOD_VERSION_BLINGUIST; }
	virtual YYCBML_CKSTRING GetName() override { return "Ballance Linguist"; }
	virtual YYCBML_CKSTRING GetAuthor() override { return "2jjy, jxpxxzj, yyc12345"; }
	virtual YYCBML_CKSTRING GetDescription() override { return "Provide more languages for Ballance."; }
	DECLARE_BML_VERSION;

private:
	virtual void OnLoad() override;
	virtual void OnLoadObject(YYCBML_CKSTRING filename, YYCBML_BOOL isMap, YYCBML_CKSTRING masterName,
		CK_CLASSID filterClass, YYCBML_BOOL addtoscene, YYCBML_BOOL reuseMeshes, YYCBML_BOOL reuseMaterials,
		YYCBML_BOOL dynamic, XObjectArray* objArray, CKObject* masterObj) override;
	virtual void OnLoadScript(YYCBML_CKSTRING filename, CKBehavior* script) override;
	virtual void OnProcess() override;
	virtual void OnExitGame() override;
	//virtual void OnPostLoadLevel() override;

	void GetFontCraftSettingsCallback(std::nullptr_t, FontCraftSettings settings);
	void CleanLanguagesNmo(CKDataArray* language);
	void EditTutorialLoadingSkip(CKBehavior* script);
	void CreateLabels(void);
	void DestroyLabels(void);

	IProperty* mCfgCore_Enabled, * mCfgCore_Language;
	IProperty* mCfgFont_Name, * mCfgFont_Size, * mCfgFont_FontCraftSync;

	NSBLinguist::LabelManager::LabelsCollection* mLabelsCollection;
	BLinguistSettings mLaunchSettings;
};

