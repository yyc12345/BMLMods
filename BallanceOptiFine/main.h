#pragma once

#include <YYCHelper.h>

extern "C" {
	__declspec(dllexport) IMod* BMLEntry(IBML* bml);
}

class BallanceOptiFine : public IMod {
	public:
	BallanceOptiFine(IBML* bml) : IMod(bml) {}

	virtual YYCBML_CKSTRING GetID() override { return "BallanceOptiFine"; }
	virtual YYCBML_CKSTRING GetVersion() override { return YYCMOD_VERSION_BALLANCEOPTIFINE; }
	virtual YYCBML_CKSTRING GetName() override { return "Ballance OptiFine"; }
	virtual YYCBML_CKSTRING GetAuthor() override { return "yyc12345"; }
	virtual YYCBML_CKSTRING GetDescription() override { return "Add shadow, transitional column and etc for old Ballance custom maps."; }
	DECLARE_BML_VERSION;

	private:
	virtual void OnLoad() override;
	virtual void OnLoadObject(YYCBML_CKSTRING filename, YYCBML_BOOL isMap, YYCBML_CKSTRING masterName,
		CK_CLASSID filterClass, YYCBML_BOOL addtoscene, YYCBML_BOOL reuseMeshes, YYCBML_BOOL reuseMaterials,
		YYCBML_BOOL dynamic, XObjectArray* objArray, CKObject* masterObj) override;

	IProperty* m_enable_props[4];
	IProperty* m_shadow_props[3];
	IProperty* m_material_props[5];
};