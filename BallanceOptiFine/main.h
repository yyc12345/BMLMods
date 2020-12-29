#pragma once

#include <BML/BMLAll.h>

extern "C" {
	__declspec(dllexport) IMod* BMLEntry(IBML* bml);
}

class BallanceOptiFine : public IMod {
	public:
	BallanceOptiFine(IBML* bml) : IMod(bml) {}

	virtual CKSTRING GetID() override { return "BallanceOptiFine"; }
	virtual CKSTRING GetVersion() override { return BML_VERSION; }
	virtual CKSTRING GetName() override { return "Ballance OptiFine"; }
	virtual CKSTRING GetAuthor() override { return "yyc12345"; }
	virtual CKSTRING GetDescription() override { return "Add shadow, transitional column for old Ballance custom maps."; }
	DECLARE_BML_VERSION;

	private:
	virtual void OnLoad() override;
	virtual void OnLoadObject(CKSTRING filename, BOOL isMap, CKSTRING masterName,
		CK_CLASSID filterClass, BOOL addtoscene, BOOL reuseMeshes, BOOL reuseMaterials,
		BOOL dynamic, XObjectArray* objArray, CKObject* masterObj) override;

	IProperty* m_enable_props[3];
	IProperty* m_shadow_props[3];
	IProperty* m_material_props[5];
};