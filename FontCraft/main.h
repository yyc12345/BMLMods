#pragma once

#include <BML/BMLAll.h>
#include <gdiplus.h>
#include <vector>
#include <string>

using namespace Gdiplus;

extern "C" {
	__declspec(dllexport) IMod* BMLEntry(IBML* bml);
}

struct FontData {
	float ustart;
	float vstart;
	float uwidth;
	float uprewidth;
	float upostwidth;
	float vwidth;
};

class FontCraft : public IMod {
	public:
	FontCraft(IBML* bml) : 
		IMod(bml),
		gdiplusStartupInput(),
		ckarrayData()
	{}

	virtual CKSTRING GetID() override { return "FontCraft"; }
	virtual CKSTRING GetVersion() override { return BML_VERSION; }
	virtual CKSTRING GetName() override { return "Font Craft"; }
	virtual CKSTRING GetAuthor() override { return "yyc12345"; }
	virtual CKSTRING GetDescription() override { return "Change Ballance Font."; }
	DECLARE_BML_VERSION;

	private:
	virtual void OnLoad() override;
	virtual void OnUnload() override;
	virtual void OnLoadObject(CKSTRING filename, BOOL isMap, CKSTRING masterName,
		CK_CLASSID filterClass, BOOL addtoscene, BOOL reuseMeshes, BOOL reuseMaterials,
		BOOL dynamic, XObjectArray* objArray, CKObject* masterObj) override;

	IProperty* m_core_props[6];

	ULONG_PTR gdiplusToken;
	GdiplusStartupInput gdiplusStartupInput;
	Bitmap* bitmapFont;
	std::vector<FontData*> ckarrayData;

};

