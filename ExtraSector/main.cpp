#include "main.h"
#include <string>
#include <stdexcept>
#include <YYCHelper.h>

IMod* BMLEntry(IBML* bml) {
	return new ExtraSector(bml);
}

//void ExtraSector::OnPreLoadLevel() {
//
//}

void ExtraSector::OnLoadObject(YYCBML_CKSTRING filename, YYCBML_BOOL isMap, YYCBML_CKSTRING masterName,
	CK_CLASSID filterClass, YYCBML_BOOL addtoscene, YYCBML_BOOL reuseMeshes, YYCBML_BOOL reuseMaterials,
	YYCBML_BOOL dynamic, XObjectArray* objArray, CKObject* masterObj) {
	// only active for map
	if (!isMap) return;

	// process attribute
	std::string sector_group_name;
	CKGroup* sector_group = NULL;
	CKBeObject* obj = NULL;
	CKContext* ctx = YYCBML_VISITOR->GetCKContext();
	CKAttributeManager* attr_mgr = ctx->GetAttributeManager();

	// get destroy dep
	CKDependencies del_dep;
	del_dep.m_Flags = CK_DEPENDENCIES_NONE;

	// get parameter type first
	CKAttributeType attr_type = attr_mgr->GetAttributeTypeByName("Sector");
	if (attr_type == -1) {
		GetLogger()->Error("Fail to get Attribute 'Sector'.");
		return;
	}

	// iterate Sector_XX group
	detected_level = 8;
	for (int index = 9; index < 9 + 1000; ++index) {
		// get group
		YYCHelper::StringHelper::StdstringPrintf(sector_group_name, "Sector_%d", index);
		sector_group = (CKGroup*)ctx->GetObjectByNameAndClass(YYCBML_TOCKSTRING(sector_group_name.c_str()), CKCID_GROUP, NULL);
		if (sector_group == NULL) {
			GetLogger()->Info("Attribute modify ok. Exit with sector: %d", index - 1);
			detected_level = index - 1;
			break;
		}

		// iterate group
		for (int gp_index = 0; gp_index < sector_group->GetObjectCount(); ++gp_index) {
			obj = sector_group->GetObject(gp_index);

			// set attribute, let vt create parameter automatically
			obj->SetAttribute(attr_type, NULL);
			// then we set it
			obj->GetAttributeParameter(attr_type)->SetValue(&index, sizeof(int));
		}

		// delete this ckgroup no dependency
		ctx->DestroyObject(sector_group, 0, &del_dep);
	}

}

void ExtraSector::OnPostLoadLevel() {
	// modify PE_Levelend
	std::string ph_name;

	// this mod do not need to process sector lower than 8
	if (detected_level <= 8) return;

	CKContext* ctx = YYCBML_VISITOR->GetCKContext();
	CKDataArray* ph = (CKDataArray*)ctx->GetObjectByNameAndClass("PH", CKCID_DATAARRAY, NULL);

	if (ph == NULL) {
		GetLogger()->Error("Fail to get CKDataArray PH. PE_Levelende will not be processed.");
		return;
	}

	// the last item is PE_Levelende
	int rows = ph->GetRowCount();
	int alloc = ph->GetElementStringValue(rows - 1, 1, NULL);
	ph_name.resize(alloc);
	ph->GetElementStringValue(rows - 1, 1, ph_name.data());

	// gotten string have extra NULL char, so we need use strcmp, 
	// not vanilla std cmp function
	if (strcmp(ph_name.c_str(), "PE_Levelende")) {
		GetLogger()->Error("Fail to get PE_Levelende entry in PH CKDataArray. Get '%s', expecting 'PE_Levelende'. PE_Levelende will not be processed.", ph_name.c_str());
		return;
	}

	// proc gotten PE_Levelende
	ph->SetElementValue(rows - 1, 0, &detected_level, sizeof(int));
	GetLogger()->Info("Process PE_Levelende OK.");
}
