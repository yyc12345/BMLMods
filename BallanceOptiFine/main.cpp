#include "main.h"
#include "utils.h"

IMod* BMLEntry(IBML* bml) {
	return new BallanceOptiFine(bml);
}

void BallanceOptiFine::OnLoad() {
	// load config
	// load enable
	GetConfig()->SetCategoryComment("OptiFine", "BallanceOptiFine enable settings");

	m_enable_props[0] = GetConfig()->GetProperty("OptiFine", "GlobalEnable");
	m_enable_props[0]->SetComment("Gloablly enable BallanceOptiFine.");
	m_enable_props[0]->SetDefaultBoolean(false);

	m_enable_props[1] = GetConfig()->GetProperty("OptiFine", "Shadow");
	m_enable_props[1]->SetComment("Enable forced adding shadow.");
	m_enable_props[1]->SetDefaultBoolean(true);

	m_enable_props[2] = GetConfig()->GetProperty("OptiFine", "Material");
	m_enable_props[2]->SetComment("Enable transitional column.");
	m_enable_props[2]->SetDefaultBoolean(true);

	m_enable_props[3] = GetConfig()->GetProperty("OptiFine", "CK2_3DOverride");
	m_enable_props[3]->SetComment("Override CK2_3D.ini TextureVideoFormat setting. Need restart game.");
	m_enable_props[3]->SetDefaultBoolean(false);

	// load shadow settings
	GetConfig()->SetCategoryComment("Shadow", "BallanceOptiFine shadow settings");

	m_shadow_props[0] = GetConfig()->GetProperty("Shadow", "Shadow");
	m_shadow_props[0]->SetComment("Add shadow for the objects grouped in Shadow.");
	m_shadow_props[0]->SetDefaultBoolean(true);

	m_shadow_props[1] = GetConfig()->GetProperty("Shadow", "Phys_Floors");
	m_shadow_props[1]->SetComment("Add shadow for the objects grouped in Phys_Floors.");
	m_shadow_props[1]->SetDefaultBoolean(true);

	m_shadow_props[2] = GetConfig()->GetProperty("Shadow", "Phys_FloorRails");
	m_shadow_props[2]->SetComment("Add shadow for the objects grouped in Phys_FloorRails.");
	m_shadow_props[2]->SetDefaultBoolean(false);

	// load material settings
	GetConfig()->SetCategoryComment("Material", "BallanceOptiFine material settings");

	m_material_props[0] = GetConfig()->GetProperty("Material", "Column_beige_fade.tga");
	m_material_props[0]->SetComment("Process Column_beige_fade.tga");
	m_material_props[0]->SetDefaultBoolean(true);

	m_material_props[1] = GetConfig()->GetProperty("Material", "Laterne_Schatten.tga");
	m_material_props[1]->SetComment("Process Laterne_Schatten.tga");
	m_material_props[1]->SetDefaultBoolean(true);

	m_material_props[2] = GetConfig()->GetProperty("Material", "Laterne_Verlauf.tga");
	m_material_props[2]->SetComment("Process Laterne_Verlauf.tga");
	m_material_props[2]->SetDefaultBoolean(true);

	m_material_props[3] = GetConfig()->GetProperty("Material", "Modul18_Gitter.tga");
	m_material_props[3]->SetComment("Process Modul18_Gitter.tga");
	m_material_props[3]->SetDefaultBoolean(false);

	m_material_props[4] = GetConfig()->GetProperty("Material", "Stick_Bottom.tga");
	m_material_props[4]->SetComment("Process Stick_Bottom.tga");
	m_material_props[4]->SetDefaultBoolean(true);


	// CK_3D render config
	if (m_enable_props[0]->GetBoolean() && m_enable_props[3]->GetBoolean()) {
		VxImageDescEx imgs;
		VxPixelFormat2ImageDesc(_32_ARGB8888, imgs);
		YYCBML_VISITOR->GetRenderManager()->SetDesiredTexturesVideoFormat(imgs);
		//m_bml->GetRenderManager()->SetRenderOptions("TextureVideoFormat", _32_ARGB8888);
	}
}

void BallanceOptiFine::OnLoadObject(YYCBML_CKSTRING filename, YYCBML_BOOL isMap, YYCBML_CKSTRING masterName,
	CK_CLASSID filterClass, YYCBML_BOOL addtoscene, YYCBML_BOOL reuseMeshes, YYCBML_BOOL reuseMaterials,
	YYCBML_BOOL dynamic, XObjectArray* objArray, CKObject* masterObj) {
	if (!isMap) return;
	GetLogger()->Info("OptiFine detect loading map. Start processing.");
	if (!m_enable_props[0]->GetBoolean()) return;

	// shadow
	if (m_enable_props[1]->GetBoolean()) {
		// first, make sure a Shadow is exist
		if (YYCBML_VISITOR->GetGroupByName("Shadow") == NULL)
			YYCBML_VISITOR->GetCKContext()->CreateObject(CKCID_GROUP, "Shadow");
		
		CKGroup* GroupShadow = YYCBML_VISITOR->GetGroupByName("Shadow");
		CKGroup* GroupPhysFloors = YYCBML_VISITOR->GetGroupByName("Phys_Floors");
		CKGroup* GroupPhysFloorRails = YYCBML_VISITOR->GetGroupByName("Phys_FloorRails");

		// Shadow group
		if (m_shadow_props[0]->GetBoolean())
			AddShadowForGroup(GroupShadow);
		// Phys_Floors group
		if (m_shadow_props[1]->GetBoolean()) {
			AddShadowForGroup(GroupPhysFloors);
			CopyToAnotherGroup(GroupPhysFloors, GroupShadow);
		}
		// Phys_FloorRails group
		if (m_shadow_props[2]->GetBoolean()) {
			AddShadowForGroup(GroupPhysFloorRails);
			CopyToAnotherGroup(GroupPhysFloorRails, GroupShadow);
		}
			
	} 
	
	// process material
	if (m_enable_props[2]->GetBoolean()) {
		// iterate loaded object
		CKMaterial* material = NULL;
		CKObject* obj = NULL;
		CKContext* ctx = YYCBML_VISITOR->GetCKContext();
		CKRenderContext* renderctx = YYCBML_VISITOR->GetRenderManager()->GetRenderContext(0);
		std::string texture_name;

		for (auto it = objArray->Begin(); it != objArray->End(); ++it) {
			obj = ctx->GetObject(*it);
			if (obj->GetClassID() != CKCID_MATERIAL) continue;
			material = (CKMaterial*)obj;

			GetMaterialTextureName(material, &texture_name);
			if (texture_name.empty()) continue;
			
			// match data
			if (texture_name == "Column_beige_fade.tga" && m_material_props[0]->GetBoolean()) {
				material->EnableAlphaTest(FALSE);
				material->SetAlphaFunc(VXCMP_GREATER);
				material->SetAlphaRef(1);
				material->EnableAlphaBlend(TRUE);
				material->SetSourceBlend(VXBLEND_SRCALPHA);
				material->SetDestBlend(VXBLEND_INVSRCALPHA);
				material->EnableZWrite(TRUE);
				material->SetZFunc(VXCMP_LESSEQUAL);

				ChangeMaterialTextureVideoFormat(material, renderctx);
			} else if (texture_name == "Laterne_Schatten.tga" && m_material_props[1]->GetBoolean()) {
				material->EnableAlphaTest(TRUE);
				material->SetAlphaFunc(VXCMP_GREATER);
				material->SetAlphaRef(1);
				material->EnableAlphaBlend(TRUE);
				material->SetSourceBlend(VXBLEND_SRCALPHA);
				material->SetDestBlend(VXBLEND_INVSRCALPHA);
				material->EnableZWrite(TRUE);
				material->SetZFunc(VXCMP_LESSEQUAL);

				ChangeMaterialTextureVideoFormat(material, renderctx);
			} else if (texture_name == "Laterne_Verlauf.tga" && m_material_props[2]->GetBoolean()) {
				material->EnableAlphaTest(TRUE);
				material->SetAlphaFunc(VXCMP_GREATER);
				material->SetAlphaRef(1);
				material->EnableAlphaBlend(TRUE);
				material->SetSourceBlend(VXBLEND_SRCALPHA);
				material->SetDestBlend(VXBLEND_INVSRCALPHA);
				material->EnableZWrite(TRUE);
				material->SetZFunc(VXCMP_LESSEQUAL);
				material->SetTwoSided(TRUE);

				ChangeMaterialTextureVideoFormat(material, renderctx);
			} else if (texture_name == "Modul18_Gitter.tga" && m_material_props[3]->GetBoolean()) {
				material->EnableAlphaTest(TRUE);
				material->SetAlphaFunc(VXCMP_GREATER);
				material->SetAlphaRef(1);
				material->EnableAlphaBlend(TRUE);
				material->SetSourceBlend(VXBLEND_SRCALPHA);
				material->SetDestBlend(VXBLEND_INVSRCALPHA);
				material->EnableZWrite(TRUE);
				material->SetZFunc(VXCMP_LESSEQUAL);

				ChangeMaterialTextureVideoFormat(material, renderctx);
			} else if (texture_name == "Stick_Bottom.tga" && m_material_props[4]->GetBoolean()) {
				material->EnableAlphaTest(FALSE);
				material->SetAlphaFunc(VXCMP_GREATER);
				material->SetAlphaRef(1);
				material->EnableAlphaBlend(TRUE);
				material->SetSourceBlend(VXBLEND_SRCALPHA);
				material->SetDestBlend(VXBLEND_INVSRCALPHA);
				material->EnableZWrite(TRUE);
				material->SetZFunc(VXCMP_LESSEQUAL);

				ChangeMaterialTextureVideoFormat(material, renderctx);
			}
		}
	}
}

