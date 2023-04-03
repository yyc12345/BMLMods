#pragma once

#include <BML/BMLAll.h>
#include "../FontCraft/main.h"
#include "LangManager.h"
#include "LabelManager.h"

struct BLinguistSettings {
	bool mEnabled;
	NSBLinguist::LangManager::UITrCollection mUITr;
	NSBLinguist::LangManager::TutorialTrCollection mTutorialTr;

	std::string FontName;
	int FontSize;
};

class BLinguist : public IMod {
public:
	BLinguist(IBML* bml) :
		IMod(bml), mLaunchSettings(), mLabelsCollection(nullptr) {
	}

	virtual CKSTRING GetID() override { return "BLinguist"; }
	virtual CKSTRING GetVersion() override { return BML_VERSION; }
	virtual CKSTRING GetName() override { return "Ballance Linguist"; }
	virtual CKSTRING GetAuthor() override { return "2jjy, jxpxxzj, yyc12345"; }
	virtual CKSTRING GetDescription() override { return "Provide more languages for Ballance."; }
	DECLARE_BML_VERSION;

private:
	virtual void OnLoad() override;
	virtual void OnLoadObject(CKSTRING filename, BOOL isMap, CKSTRING masterName,
		CK_CLASSID filterClass, BOOL addtoscene, BOOL reuseMeshes, BOOL reuseMaterials,
		BOOL dynamic, XObjectArray* objArray, CKObject* masterObj) override;
	virtual void OnLoadScript(CKSTRING filename, CKBehavior* script) override;
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
