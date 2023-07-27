#pragma once

#include <YYCHelper.h>

extern "C" {
	__declspec(dllexport) IMod* BMLEntry(IBML* bml);
}
class ExtraSector : public IMod {
public:
	ExtraSector(IBML* bml) :
		IMod(bml),
		detected_level(8) {
	}

	virtual YYCBML_CKSTRING GetID() override { return "ExtraSector"; }
	virtual YYCBML_CKSTRING GetVersion() override { return YYCMOD_VERSION_EXTRASECTOR; }
	virtual YYCBML_CKSTRING GetName() override { return "Extra Sector"; }
	virtual YYCBML_CKSTRING GetAuthor() override { return "2jjy, Gamepiaynmo, yyc12345"; }
	virtual YYCBML_CKSTRING GetDescription() override { return "Yet Another 999 Sector Loader."; }
	DECLARE_BML_VERSION;

private:
	int detected_level;

	virtual void OnLoadObject(YYCBML_CKSTRING filename, YYCBML_BOOL isMap, YYCBML_CKSTRING masterName,
		CK_CLASSID filterClass, YYCBML_BOOL addtoscene, YYCBML_BOOL reuseMeshes, YYCBML_BOOL reuseMaterials,
		YYCBML_BOOL dynamic, XObjectArray* objArray, CKObject* masterObj) override;
	virtual void OnPostLoadLevel() override;
};

