#include "main.h"
#include <string>
#include <stdexcept>

//void StdstringVPrintf(std::string* strl, const char* format, va_list argptr) {
//	int count = _vsnprintf(NULL, 0, format, argptr);
//	count++;
//
//	strl->resize(count);
//	(*strl)[count - 1] = '\0';
//	int write_result = _vsnprintf(strl->data(), count, format, argptr);
//
//	if (write_result < 0 || write_result >= count) throw new std::length_error("Invalid write_result in _vsnprintf.");
//}
//void StdstringPrintf(std::string* strl, const char* format, ...) {
//	va_list argptr;
//	va_start(argptr, format);
//	StdstringVPrintf(strl, format, argptr);
//	va_end(argptr);
//}

extern "C" {
	__declspec(dllexport) IMod* BMLEntry(IBML* bml) {
		return new BLinguist(bml);
	}
}

void BLinguist::OnLoad() {
	// load settings
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
	mCfgFont_Size->SetDefaultInteger(10);

	mCfgFont_FontCraftSync = GetConfig()->GetProperty("Font", "FontCraft");
	mCfgFont_FontCraftSync->SetComment("Try getting settings from FontCraft.");
	mCfgFont_FontCraftSync->SetDefaultBoolean(true);

	// fill launch settings
	mLaunchSettings.mEnabled = mCfgCore_Enabled->GetBoolean();
	mLaunchSettings.Langauge = mCfgCore_Language->GetString();
	mLaunchSettings.FontName = mCfgFont_Name->GetString();
	mLaunchSettings.FontSize = mCfgFont_Size->GetInteger();

	// try interacte with FontCraft
	if (mCfgFont_FontCraftSync->GetBoolean()) {
		bool failed = true;
		int modcount = m_bml->GetModCount();
		for (int i = 0; i < modcount; ++i) {
			IMod* modinstance = m_bml->GetMod(i);
			if (!YYCHelper::StringHelper::CKStringEqual(modinstance->GetID(), "FontCraft")) continue;
			if (!YYCHelper::StringHelper::CKStringEqual(modinstance->GetVersion(), BML_VERSION)) continue;

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

void BLinguist::GetFontCraftSettingsCallback(std::nullptr_t t, FontCraftSettings settings) {
	mLaunchSettings.FontName = settings.mFontName;
	mLaunchSettings.FontSize = (int)settings.mFontSize;

	GetLogger()->Info("Communicate with FontCraft success! Font name: \"%s\". Font size: %d", 
		mLaunchSettings.FontName.c_str(), mLaunchSettings.FontSize);
}

//void ExtraSector::OnPreLoadLevel() {
//
//}
//
//void BLinguist::OnLoadObject(CKSTRING filename, BOOL isMap, CKSTRING masterName,
//	CK_CLASSID filterClass, BOOL addtoscene, BOOL reuseMeshes, BOOL reuseMaterials,
//	BOOL dynamic, XObjectArray* objArray, CKObject* masterObj) {
//	// only active for map
//	if (!isMap) return;
//
//	// process attribute
//	std::string sector_group_name;
//	CKGroup* sector_group = NULL;
//	CKBeObject* obj = NULL;
//	CKContext* ctx = m_bml->GetCKContext();
//	CKAttributeManager* attr_mgr = ctx->GetAttributeManager();
//
//	// get destroy dep
//	CKDependencies del_dep;
//	del_dep.m_Flags = CK_DEPENDENCIES_NONE;
//
//	// get parameter type first
//	CKAttributeType attr_type = attr_mgr->GetAttributeTypeByName("Sector");
//	if (attr_type == -1) {
//		GetLogger()->Error("Fail to get Attribute 'Sector'.");
//		return;
//	}
//
//	// iterate Sector_XX group
//	detected_level = 8;
//	for (int index = 9; index < 9 + 1000; ++index) {
//		// get group
//		StdstringPrintf(&sector_group_name, "Sector_%d", index);
//		sector_group = (CKGroup*)ctx->GetObjectByNameAndClass(sector_group_name.c_str(), CKCID_GROUP, NULL);
//		if (sector_group == NULL) {
//			GetLogger()->Info("Attribute modify ok. Exit with sector: %d", index - 1);
//			detected_level = index - 1;
//			break;
//		}
//
//		// iterate group
//		for (int gp_index = 0; gp_index < sector_group->GetObjectCount(); ++gp_index) {
//			obj = sector_group->GetObject(gp_index);
//
//			// set attribute, let vt create parameter automatically
//			obj->SetAttribute(attr_type, NULL);
//			// then we set it
//			obj->GetAttributeParameter(attr_type)->SetValue(&index, sizeof(int));
//		}
//
//		// delete this ckgroup no dependency
//		ctx->DestroyObject(sector_group, 0, &del_dep);
//	}
//
//}
//
//void BLinguist::OnPostLoadLevel() {
//	// modify PE_Levelend
//	std::string ph_name;
//
//	// this mod do not need to process sector lower than 8
//	if (detected_level <= 8) return;
//
//	CKContext* ctx = m_bml->GetCKContext();
//	CKDataArray* ph = (CKDataArray*)ctx->GetObjectByNameAndClass("PH", CKCID_DATAARRAY, NULL);
//
//	if (ph == NULL) {
//		GetLogger()->Error("Fail to get CKDataArray PH. PE_Levelende will not be processed.");
//		return;
//	}
//
//	// the last item is PE_Levelende
//	int rows = ph->GetRowCount();
//	int alloc = ph->GetElementStringValue(rows - 1, 1, NULL);
//	ph_name.resize(alloc);
//	ph->GetElementStringValue(rows - 1, 1, ph_name.data());
//
//	// gotten string have extra NULL char, so we need use strcmp, 
//	// not vanilla std cmp function
//	if (strcmp(ph_name.c_str(), "PE_Levelende")) {
//		GetLogger()->Error("Fail to get PE_Levelende entry in PH CKDataArray. Get '%s', expecting 'PE_Levelende'. PE_Levelende will not be processed.", ph_name.c_str());
//		return;
//	}
//
//	// proc gotten PE_Levelende
//	ph->SetElementValue(rows - 1, 0, &detected_level, sizeof(int));
//	GetLogger()->Info("Process PE_Levelende OK.");
//}
