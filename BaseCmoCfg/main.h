#pragma once

#include <BML/BMLAll.h>
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

	virtual CKSTRING GetID() override { return "BaseCmoCfg"; }
	virtual CKSTRING GetVersion() override { return BML_VERSION; }
	virtual CKSTRING GetName() override { return "Base Cmo Config"; }
	virtual CKSTRING GetAuthor() override { return "yyc12345"; }
	virtual CKSTRING GetDescription() override { return "Change GameSettings array located in Base.cmo. Almost settings need restart game to apply!"; }
	DECLARE_BML_VERSION;

private:
	virtual void OnLoad() override;
	virtual void OnLoadObject(CKSTRING filename, BOOL isMap, CKSTRING masterName,
		CK_CLASSID filterClass, BOOL addtoscene, BOOL reuseMeshes, BOOL reuseMaterials,
		BOOL dynamic, XObjectArray* objArray, CKObject* masterObj) override;

	IProperty* m_core_props[2];


};

