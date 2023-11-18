#pragma once

#include <YYCHelper.h>

extern "C" {
	__declspec(dllexport) IMod* BMLEntry(IBML* bml);
}
class BallanceBBOR : public IMod {
public:
	BallanceBBOR(IBML* bml) :
		IMod(bml) {}
	
	virtual YYCBML_CKSTRING GetID() override { return "BallanceBBOR"; }
	virtual YYCBML_CKSTRING GetVersion() override { return YYCMOD_VERSION_BALLANCEBBOR; }
	virtual YYCBML_CKSTRING GetName() override { return "Ballance Bounding Box Outline Reloaded"; }
	virtual YYCBML_CKSTRING GetAuthor() override { return "yyc12345"; }
	virtual YYCBML_CKSTRING GetDescription() override { return "Show depth cubes, transformer sphere, ventilator area and more."; }
	DECLARE_BML_VERSION;

private:
	virtual void OnLoad() override;
	virtual void OnUnload() override;
	virtual void OnLoadObject(YYCBML_CKSTRING filename, YYCBML_BOOL isMap, YYCBML_CKSTRING masterName,
		CK_CLASSID filterClass, YYCBML_BOOL addtoscene, YYCBML_BOOL reuseMeshes, YYCBML_BOOL reuseMaterials,
		YYCBML_BOOL dynamic, XObjectArray* objArray, CKObject* masterObj) override;

};

