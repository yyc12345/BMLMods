#include "utils.h"
#include <filesystem>

void AddShadowForGroup(CKGroup* grp) {
	if (grp == NULL) return;

	CKBeObject* cache = NULL;
	CK3dEntity* obj = NULL;
	int count = grp->GetObjectCount();
	CK_CLASSID objClass;
	for (int i = 0; i < count; i++) {
		cache = grp->GetObject(i);
		objClass = cache->GetClassID();
		if (objClass == CKCID_3DENTITY || objClass == CKCID_3DOBJECT) {
			obj = (CK3dEntity*)cache;
			obj->ModifyMoveableFlags(8, 0);
		}
	}
}

void CopyToAnotherGroup(CKGroup* group1, CKGroup* group2) {
	if (group1 == NULL || group2 == NULL) return;

	int count = group1->GetObjectCount();
	for (int i = 0; i < count; ++i)
		group2->AddObject(group1->GetObject(i));
}

void GetMaterialTextureName(CKMaterial* mat, std::string* name) {
	std::filesystem::path filepath;
	CKTexture* texture = NULL;

	name->clear();

	texture = mat->GetTexture(0);
	if (texture == NULL) return;
	if (texture->GetSlotFileName(0) == NULL) return;

	filepath = texture->GetSlotFileName(0);
	*name = filepath.filename().string();
}

void ChangeMaterialTextureVideoFormat(CKMaterial* mat) {
	CKTexture* texture = NULL;
	texture = mat->GetTexture(0);
	if (texture == NULL) return;
	texture->SetDesiredVideoFormat(_32_ARGB8888);
}