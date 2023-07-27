#pragma once

#include <YYCHelper.h>
#include <vector>
#include <string>

extern "C" {
	__declspec(dllexport) IMod* BMLEntry(IBML* bml);
}
class BaseCmoCfg : public IMod {
public:
	BaseCmoCfg(IBML* bml) :
		IMod(bml) {
	}

	virtual YYCBML_CKSTRING GetID() override { return "BaseCmoCfg"; }
	virtual YYCBML_CKSTRING GetVersion() override { return YYCMOD_VERSION_BASECMOCFG; }
	virtual YYCBML_CKSTRING GetName() override { return "Base Cmo Config"; }
	virtual YYCBML_CKSTRING GetAuthor() override { return "yyc12345"; }
	virtual YYCBML_CKSTRING GetDescription() override { return "Change GameSettings array located in Base.cmo. Almost settings need restart game to apply!"; }
	DECLARE_BML_VERSION;

private:
	virtual void OnLoad() override;
	virtual void OnLoadObject(YYCBML_CKSTRING filename, YYCBML_BOOL isMap, YYCBML_CKSTRING masterName,
		CK_CLASSID filterClass, YYCBML_BOOL addtoscene, YYCBML_BOOL reuseMeshes, YYCBML_BOOL reuseMaterials,
		YYCBML_BOOL dynamic, XObjectArray* objArray, CKObject* masterObj) override;

	IProperty* m_core_props[2];


};

