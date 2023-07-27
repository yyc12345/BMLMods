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

	IProperty* m_core_props[6];

	ULONG_PTR gdiplusToken;
	GdiplusStartupInput gdiplusStartupInput;
	Bitmap* bitmapFont;
	std::vector<FontData*> ckarrayData;

public:
	virtual BML_CKSTRING GetID() override { return "FontCraft"; }
	virtual BML_CKSTRING GetVersion() override { return BML_VERSION; }
	virtual BML_CKSTRING GetName() override { return "Font Craft"; }
	virtual BML_CKSTRING GetAuthor() override { return "yyc12345"; }
	virtual BML_CKSTRING GetDescription() override { return "Change Ballance Font."; }
	DECLARE_BML_VERSION;

public:
	virtual void TryGetFontCraftSettings(EventOnLoad_t::ListenerCallback callback);

private:
	virtual void OnLoad() override;
	virtual void OnUnload() override;
	virtual void OnLoadObject(BML_CKSTRING filename, BML_BOOL isMap, BML_CKSTRING masterName,
		CK_CLASSID filterClass, BML_BOOL addtoscene, BML_BOOL reuseMeshes, BML_BOOL reuseMaterials,
		BML_BOOL dynamic, XObjectArray* objArray, CKObject* masterObj) override;


};

