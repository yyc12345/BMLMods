#include "BLinguist.h"
#include <string>
#include <stdexcept>

extern "C" {
	__declspec(dllexport) IMod* BMLEntry(IBML* bml) {
		return new BLinguist(bml);
	}
}

void BLinguist::OnLoad() {
	// ========== load settings ==========
	GetConfig()->SetCategoryComment("Core", "Core settings for BLinguist");

	mCfgCore_Enabled = GetConfig()->GetProperty("Core", "Enable");
	mCfgCore_Enabled->SetComment("Globally enable BLinguist");
	mCfgCore_Enabled->SetDefaultBoolean(false);

	mCfgCore_Language = GetConfig()->GetProperty("Core", "Language");
	mCfgCore_Language->SetComment("Display Language");
	mCfgCore_Language->SetDefaultString("");

	GetConfig()->SetCategoryComment("Font", "Font settings for BLinguist");

	mCfgFont_Name = GetConfig()->GetProperty("Font", "FontName");
	mCfgFont_Name->SetComment("Font name");
	mCfgFont_Name->SetDefaultString("Arial");

	mCfgFont_Size = GetConfig()->GetProperty("Font", "FontSize");
	mCfgFont_Size->SetComment("Font size");
	mCfgFont_Size->SetDefaultInteger(12);

	mCfgFont_FontCraftSync = GetConfig()->GetProperty("Font", "FontCraft");
	mCfgFont_FontCraftSync->SetComment("Try getting settings from FontCraft.");
	mCfgFont_FontCraftSync->SetDefaultBoolean(true);

	// ========== try detecting user environment ==========
	// only try it when language field is invalid (almost in first start)
	NSBLinguist::LangManager::LangIdentifier lang_ident(mCfgCore_Language->GetString());
	if (!NSBLinguist::LangManager::IsLegalLangIdentifier(lang_ident)) {
		if (NSBLinguist::LangManager::DetectLanguages(lang_ident)) {
			// success detect. enable plugin in default
			mCfgCore_Enabled->SetBoolean(true);
		} else {
			// fail to detect. disable plugin in default
			mCfgCore_Enabled->SetBoolean(false);
			GetLogger()->Warn("Fail to detect language. Plugin is disabled now.");
		}

		// always set language prop
		mCfgCore_Language->SetString(lang_ident.c_str());
	}

	// ========== fill launch settings ==========
	// load translation first
	if (mCfgCore_Enabled->GetBoolean()) {
		if (!NSBLinguist::LangManager::LoadTranslations(lang_ident, mLaunchSettings.mUITr, mLaunchSettings.mTutorialTr)) {
			// something happend. fail to load. disable plugin anyway
			mCfgCore_Enabled->SetBoolean(false);
			GetLogger()->Warn("Fail to load translations. Plugin is disabled now.");
		}
	}
	// load other settings
	mLaunchSettings.mEnabled = mCfgCore_Enabled->GetBoolean();
	mLaunchSettings.FontName = mCfgFont_Name->GetString();
	mLaunchSettings.FontSize = YYCHelper::DpiHelper::GetDpiScaledFontSize(YYCBML_VISITOR->GetCKContext(), static_cast<float>(mCfgFont_Size->GetInteger()));
	mLaunchSettings.FontColor = 0xFFFFFFFF;	// default color is white

	// ========== try interacte with FontCraft ==========
	if (mCfgFont_FontCraftSync->GetBoolean()) {
		bool failed = true;
		int modcount = YYCBML_VISITOR->GetModCount();
		for (int i = 0; i < modcount; ++i) {
			IMod* modinstance = YYCBML_VISITOR->GetMod(i);
			if (!YYCHelper::StringHelper::CKStringEqual(modinstance->GetID(), "FontCraft")) continue;
			if (!YYCHelper::StringHelper::CKStringEqual(modinstance->GetVersion(), YYCMOD_VERSION_FONTCRAFT)) continue;

			// yes, this is font craft
			FontCraft* fontcraft = dynamic_cast<FontCraft*>(modinstance);
			if (fontcraft != nullptr) {
				fontcraft->TryGetFontCraftSettings(std::bind(&BLinguist::GetFontCraftSettingsCallback, this, std::placeholders::_1, std::placeholders::_2));
				failed = false;
				break;
			}
		}

		if (failed) {
			GetLogger()->Warn("Fail to get FontCraft settings. Use self settings instead.");
		}

	}
}

void BLinguist::OnLoadObject(YYCBML_CKSTRING filename, YYCBML_BOOL isMap, YYCBML_CKSTRING masterName,
	CK_CLASSID filterClass, YYCBML_BOOL addtoscene, YYCBML_BOOL reuseMeshes, YYCBML_BOOL reuseMaterials,
	YYCBML_BOOL dynamic, XObjectArray* objArray, CKObject* masterObj) {

	// do not process if disabled
	if (!mLaunchSettings.mEnabled) return;
	// do not process map
	if (isMap) return;

	// process Languages.nmo
	if (YYCHelper::StringHelper::CKStringEqual(filename, "3D Entities\\Language.nmo")) {
		CKObject* obj = YYCBML_VISITOR->GetCKContext()->GetObjectByNameAndClass("language", CKCID_DATAARRAY, NULL);
		if (obj == nullptr || obj->GetClassID() != CKCID_DATAARRAY) return;

		CleanLanguagesNmo((CKDataArray*)obj);
	} else if (YYCHelper::StringHelper::CKStringEqual(filename, "3D Entities\\Gameplay.nmo")) {
		// create labels once Gameplay.nmo has been loaded.
		CreateLabels();
	}

}

void BLinguist::OnLoadScript(YYCBML_CKSTRING filename, CKBehavior* script) {
	// do not process if disabled
	if (!mLaunchSettings.mEnabled) return;

	// process Gameplay.nmo
	if (YYCHelper::StringHelper::CKStringEqual(filename, "3D Entities\\Gameplay.nmo") &&
		YYCHelper::StringHelper::CKStringEqual(script->GetName(), "Gameplay_Tutorial")) {
		EditTutorialLoadingSkip(script);
	}
}

void BLinguist::OnProcess() {
	if (mLabelsCollection != nullptr) mLabelsCollection->Process();
}

void BLinguist::OnExitGame() {
	DestroyLabels();
}


void BLinguist::GetFontCraftSettingsCallback(std::nullptr_t t, FontCraftSettings settings) {
	// apply font name and size
	mLaunchSettings.FontName = settings.mFontName;
	mLaunchSettings.FontSize = YYCHelper::DpiHelper::GetDpiScaledFontSize(
		YYCBML_VISITOR->GetCKContext(), 
		settings.mFontSize * (16.0f / 20.0f)	// 16 is BLinguist font size. 20 is FontCraft font size.
	);
	// apply font color
	mLaunchSettings.FontColor = settings.mFontColor.GetRGBA();

	GetLogger()->Info("Communicate with FontCraft success! Font name: \"%s\". Font size: %d. Font color (ARGB): 0x%08x.", 
		mLaunchSettings.FontName.c_str(), mLaunchSettings.FontSize, mLaunchSettings.FontColor);
}

void BLinguist::CleanLanguagesNmo(CKDataArray* language) {
	int rows = language->GetRowCount();
	for (int row = 0; row < rows; ++row) {
		for (int column = 1; column < 5; ++column) {
			language->SetElementStringValue(row, column, "");
		}
	}
	GetLogger()->Info("Clean Language.nmo/language done.");
}

void BLinguist::EditTutorialLoadingSkip(CKBehavior* script) {
	CKBehavior* bbInit = ScriptHelper::FindFirstBB(script, "Init", false, 1, 1, 0, 0);
	if (bbInit == nullptr) return;
	CKBehavior* bbLoadTutorialText = ScriptHelper::FindFirstBB(bbInit, "load Tutorialtext", false, 1, 1, 0, 0);
	if (bbLoadTutorialText == nullptr) return;
	CKBehavior* bbCreateString = ScriptHelper::FindFirstBB(bbLoadTutorialText, "Create String");
	if (bbCreateString == nullptr) return;

	// remove all links from this bb output
	CKBehaviorIO* bbCreateString_CreateOut = bbCreateString->GetOutput(0);
	if (bbCreateString_CreateOut == nullptr) return;
	// backup first to prevent CK change it during for statement
	// use patched method get current link list
	XSObjectPointerArray* cklinks = YYCHelper::BMLPatches::CKBehaviorIO_GetLinks(bbCreateString_CreateOut);
	// backup
	std::vector<CKBehaviorLink*> links;
	for (int i = 0; i < cklinks->Size(); ++i) {
		links.emplace_back((CKBehaviorLink*)(*cklinks)[i]);
	}
	// then delete
	for (const auto& link : links) {
		ScriptHelper::DeleteLink(bbLoadTutorialText, link);
	}

	// create a new link
	ScriptHelper::CreateLink(bbLoadTutorialText, bbCreateString_CreateOut, bbLoadTutorialText->GetOutput(0), 0);

	GetLogger()->Info("Edit Gameplay.nmo/Gameplay_Tutorial done.");
}

void BLinguist::CreateLabels(void) {
	if (!mLaunchSettings.mEnabled) return;
	if (mLabelsCollection != nullptr) return;

	auto instance = new NSBLinguist::LabelManager::LabelsCollection(
		YYCBML_VISITOR->GetCKContext(),
		mLaunchSettings.mUITr, mLaunchSettings.mTutorialTr,
		mLaunchSettings.FontName, mLaunchSettings.FontSize, mLaunchSettings.FontColor
	);
	mLabelsCollection = instance;
	GetLogger()->Info("Create Labels Collection done!");
}

void BLinguist::DestroyLabels(void) {
	if (mLabelsCollection != nullptr) {
		auto instance = mLabelsCollection;
		mLabelsCollection = nullptr;
		delete instance;

		GetLogger()->Info("Dispose Labels Collection done!");
	}
}
