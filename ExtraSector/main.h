#pragma once

#include <BML/BMLAll.h>

extern "C" {
	__declspec(dllexport) IMod* BMLEntry(IBML* bml);
}
class ExtraSector : public IMod {
public:
	ExtraSector(IBML* bml) :
		IMod(bml) {
	}

	virtual CKSTRING GetID() override { return "ExtraSector"; }
	virtual CKSTRING GetVersion() override { return BML_VERSION; }
	virtual CKSTRING GetName() override { return "Extra Sector"; }
	virtual CKSTRING GetAuthor() override { return "yyc12345, Gamepiaynmo, 2jjy"; }
	virtual CKSTRING GetDescription() override { return "Yet Another 999 Sector Loader."; }
	DECLARE_BML_VERSION;

private:
	int detected_level;

	virtual void OnLoadObject(CKSTRING filename, BOOL isMap, CKSTRING masterName,
		CK_CLASSID filterClass, BOOL addtoscene, BOOL reuseMeshes, BOOL reuseMaterials,
		BOOL dynamic, XObjectArray* objArray, CKObject* masterObj) override;
	virtual void OnPostLoadLevel() override;
};

