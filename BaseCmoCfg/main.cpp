#include "main.h"

IMod* BMLEntry(IBML* bml) {
	return new BaseCmoCfg(bml);
}

void BaseCmoCfg::OnLoad() {
	// load settings
	GetConfig()->SetCategoryComment("Core", "Core settings of BaseCmoCfg");

	m_core_props[0] = GetConfig()->GetProperty("Core", "Godmode");
	m_core_props[0]->SetComment("The field called <Godmode?>");
	m_core_props[0]->SetDefaultBoolean(false);

	m_core_props[1] = GetConfig()->GetProperty("Core", "Debug");
	m_core_props[1]->SetComment("The field called <Debug?>");
	m_core_props[1]->SetDefaultBoolean(false);

}

void BaseCmoCfg::OnLoadObject(YYCBML_CKSTRING filename, YYCBML_BOOL isMap, YYCBML_CKSTRING masterName,
	CK_CLASSID filterClass, YYCBML_BOOL addtoscene, YYCBML_BOOL reuseMeshes, YYCBML_BOOL reuseMaterials,
	YYCBML_BOOL dynamic, XObjectArray* objArray, CKObject* masterObj) {

	// process base.cmo
	if (strcmp(filename, "base.cmo")) return;
	GetLogger()->Info("Capture base.cmo loaded event!");

	CKContext* ctx = YYCBML_VISITOR->GetCKContext();
	CKDataArray* gamesettings = (CKDataArray*)ctx->GetObjectByNameAndClass("GameSettings", CKCID_DATAARRAY, NULL);
	
	if (gamesettings == NULL) {
		GetLogger()->Error("Fail to get CKDataArray <GameSettings>!");
		return;
	}

	BOOL val = m_core_props[0]->GetBoolean();
	//CKParameter* fakeParam = (CKParameter*)ctx->CreateObject(CKCID_PARAMETER);
	//fakeParam->SetGUID(CKPGUID_BOOL);
	//fakeParam->SetValue(&val, sizeof(BOOL));
	gamesettings->SetElementValue(0, 6, &val, sizeof(BOOL));
	val = m_core_props[1]->GetBoolean();
	gamesettings->SetElementValue(0, 7, &val, sizeof(BOOL));

	GetLogger()->Info("GameSettings in base.cmo has been changed.");

	//ctx->DestroyObject(fakeParam);
}

