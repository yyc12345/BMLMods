#pragma once

#include <YYCHelper.h>

void AddShadowForGroup(CKGroup* grp);
void CopyToAnotherGroup(CKGroup* group1, CKGroup* group2);
void GetMaterialTextureName(CKMaterial* mat, std::string* name);
void ChangeMaterialTextureVideoFormat(CKMaterial* mat, CKRenderContext* renderctx);
