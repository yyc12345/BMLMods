#pragma once

#include <YYCHelper.h>
#include <gdiplus.h>
#include <vector>
#include <string>

using namespace Gdiplus;

struct FontData {
	float ustart;
	float vstart;
	float uwidth;
	float uprewidth;
	float upostwidth;
	float vwidth;
};

struct FontCraftSettings {
	bool mEnabled;
	std::string mFontName;
	float mFontSize;
	bool mIsItalic;
	bool mIsBold;
	bool mIsUnderLine;
	VxColor mFontColor;
};

class FontCraft : public IMod {
public:
	FontCraft(IBML* bml) :
		IMod(bml), mEventOnLoad(),
		gdiplusStartupInput(),
		ckarrayData() {
	}

private:
	FontCraftSettings GetFontCraftSettings(std::nullptr_t blank);
	using EventOnLoad_t = YYCHelper::ModEvents::ModEventDispatcher<std::nullptr_t, FontCraftSettings>;
	EventOnLoad_t mEventOnLoad;

	IProperty* m_core_props[7];

	ULONG_PTR gdiplusToken;
	GdiplusStartupInput gdiplusStartupInput;
	Bitmap* bitmapFont;
	std::vector<FontData*> ckarrayData;

public:
	virtual YYCBML_CKSTRING GetID() override { return "FontCraft"; }
	virtual YYCBML_CKSTRING GetVersion() override { return YYCMOD_VERSION_FONTCRAFT; }
	virtual YYCBML_CKSTRING GetName() override { return "Font Craft"; }
	virtual YYCBML_CKSTRING GetAuthor() override { return "yyc12345"; }
	virtual YYCBML_CKSTRING GetDescription() override { return "Change Ballance Font."; }
	DECLARE_BML_VERSION;

public:
	virtual void TryGetFontCraftSettings(EventOnLoad_t::ListenerCallback callback);

private:
	virtual void OnLoad() override;
	virtual void OnUnload() override;
	virtual void OnLoadObject(YYCBML_CKSTRING filename, YYCBML_BOOL isMap, YYCBML_CKSTRING masterName,
		CK_CLASSID filterClass, YYCBML_BOOL addtoscene, YYCBML_BOOL reuseMeshes, YYCBML_BOOL reuseMaterials,
		YYCBML_BOOL dynamic, XObjectArray* objArray, CKObject* masterObj) override;


};

