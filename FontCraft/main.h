#pragma once

#include <BML/BMLAll.h>
#include <gdiplus.h>
#include <vector>
#include <string>
#include <YYCHelper.h>

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
	virtual CKSTRING GetID() override { return "FontCraft"; }
	virtual CKSTRING GetVersion() override { return BML_VERSION; }
	virtual CKSTRING GetName() override { return "Font Craft"; }
	virtual CKSTRING GetAuthor() override { return "yyc12345"; }
	virtual CKSTRING GetDescription() override { return "Change Ballance Font."; }
	DECLARE_BML_VERSION;

public:
	virtual void TryGetFontCraftSettings(EventOnLoad_t::ListenerCallback callback);

private:
	virtual void OnLoad() override;
	virtual void OnUnload() override;
	virtual void OnLoadObject(CKSTRING filename, BOOL isMap, CKSTRING masterName,
		CK_CLASSID filterClass, BOOL addtoscene, BOOL reuseMeshes, BOOL reuseMaterials,
		BOOL dynamic, XObjectArray* objArray, CKObject* masterObj) override;


};

