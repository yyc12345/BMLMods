#include "main.h"

extern "C" {
	__declspec(dllexport) IMod* BMLEntry(IBML* bml) {
		return new FontCraft(bml);
	}
}

// define bitmap font const
#define HW_IMG 512
#define HW_COUNT 16
#define HW_CELL ((float)HW_IMG / (float)HW_COUNT)

FontCraftSettings FontCraft::GetFontCraftSettings(std::nullptr_t blank) {
	FontCraftSettings settings;
	settings.mEnabled = m_core_props[0]->GetBoolean();
	settings.mFontName = m_core_props[1]->GetString();
	settings.mFontSize = m_core_props[2]->GetFloat();
	settings.mIsItalic = m_core_props[3]->GetBoolean();
	settings.mIsBold = m_core_props[4]->GetBoolean();
	settings.mIsUnderLine = m_core_props[5]->GetBoolean();
	YYCHelper::ColorStringParser::ParseColorString(
		m_core_props[6]->GetString(), settings.mFontColor
	);
	return settings;
}

void FontCraft::TryGetFontCraftSettings(EventOnLoad_t::ListenerCallback callback) {
	mEventOnLoad.ListenEvent(callback, std::nullptr_t());
}

void FontCraft::OnLoad() {
	// load settings
	GetConfig()->SetCategoryComment("Core", "Core settings for FontCraft");

	m_core_props[0] = GetConfig()->GetProperty("Core", "Enable");
	m_core_props[0]->SetComment("Globally enable FontCraft");
	m_core_props[0]->SetDefaultBoolean(true);

	m_core_props[1] = GetConfig()->GetProperty("Core", "FontName");
	m_core_props[1]->SetComment("Font name");
	m_core_props[1]->SetDefaultString("Arial");

	m_core_props[2] = GetConfig()->GetProperty("Core", "FontSize");
	m_core_props[2]->SetComment("Font size?");
	m_core_props[2]->SetDefaultFloat(20.0);

	m_core_props[3] = GetConfig()->GetProperty("Core", "FontItalic");
	m_core_props[3]->SetComment("Font italic?");
	m_core_props[3]->SetDefaultBoolean(false);

	m_core_props[4] = GetConfig()->GetProperty("Core", "FontBold");
	m_core_props[4]->SetComment("Font bold?");
	m_core_props[4]->SetDefaultBoolean(false);

	m_core_props[5] = GetConfig()->GetProperty("Core", "FontUnderLine");
	m_core_props[5]->SetComment("Font underline?");
	m_core_props[5]->SetDefaultBoolean(false);

	m_core_props[6] = GetConfig()->GetProperty("Core", "FontColor");
	m_core_props[6]->SetComment("The color of font. Support 2 style (alpha is optional): RRR,GGG,BBB,AAA or #rrggbbaa. The factor of first style range from 0 to 255. The second style accept a hex number as a component from 00 to FF.");
	m_core_props[6]->SetDefaultString("#ffffff");

	// register handler
	// because props had been loaded
	mEventOnLoad.RegisterHandler(std::bind(&FontCraft::GetFontCraftSettings, this, std::placeholders::_1));

	// gdi+
	if (m_core_props[0]->GetBoolean()) {
		GetLogger()->Info("Drawing bitmap font image...");
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		bitmapFont = new Gdiplus::Bitmap(HW_IMG, HW_IMG, PixelFormat32bppARGB);
		Graphics g(bitmapFont);
		g.Clear(Gdiplus::Color(0, 255, 0, 0));
		g.SetTextRenderingHint(TextRenderingHint::TextRenderingHintAntiAliasGridFit);

		// convert input font name to wchar
		std::string inputChar;
		std::wstring inputWChar;
		inputChar = m_core_props[1]->GetString();
		if (!YYCHelper::EncodingHelper::CharToWchar(inputChar, inputWChar, CP_ACP)) {
			// convert failed, use fallback
			inputWChar = L"Arial";
		}

		int fs = FontStyle::FontStyleRegular;
		if (m_core_props[3]->GetBoolean())
			fs = fs | FontStyle::FontStyleItalic;
		if (m_core_props[4]->GetBoolean())
			fs = fs | FontStyle::FontStyleBold;
		if (m_core_props[5]->GetBoolean())
			fs = fs | FontStyle::FontStyleUnderline;
		Gdiplus::Font ft(
			inputWChar.c_str(), 
			static_cast<Gdiplus::REAL>(YYCHelper::DpiHelper::GetDpiScaledFontSize(YYCBML_VISITOR->GetCKContext(), m_core_props[2]->GetFloat())), 
			(FontStyle)fs, 
			Gdiplus::UnitPoint
		);

		VxColor fontColor;
		if (!YYCHelper::ColorStringParser::ParseColorString(m_core_props[6]->GetString(), fontColor)) {
			GetLogger()->Info("Parsing color string failed: \"%s\". use default color.", m_core_props[6]->GetString());
		}
		Gdiplus::Color gdiFontColor(static_cast<Gdiplus::ARGB>(fontColor.GetRGBA()));
		Gdiplus::SolidBrush sb(gdiFontColor);

		Gdiplus::StringFormat sf(Gdiplus::StringFormat::GenericTypographic());
		sf.SetFormatFlags(sf.GetFormatFlags() | StringFormatFlags::StringFormatFlagsMeasureTrailingSpaces);

		float x = 0, y = 0;
		Gdiplus::SizeF measureRes;
		Gdiplus::SizeF emptySize(0, 0);
		Gdiplus::SizeF drawPosition(0, 0);

		inputChar.resize(1);
		inputChar[0] = '\0';
		FontData* dataitem = NULL;
		for (int i = 0; i < HW_COUNT; ++i) {
			for (int j = 0; j < HW_COUNT; ++j) {
				// convert single 1252 char to wchar
				if (!YYCHelper::EncodingHelper::CharToWchar(inputChar, inputWChar, 1252)) {
					// convertion failed, use null fallback
					inputWChar = L"\0";
				}

				g.MeasureString(inputWChar.c_str(), 1, &ft, emptySize, &sf, &measureRes);

				drawPosition.Width = x = j * HW_CELL;
				drawPosition.Height = y = i * HW_CELL + (HW_CELL - measureRes.Height) / 2;
				dataitem = new FontData();
				dataitem->ustart = x / HW_IMG;
				dataitem->vstart = (float)i / HW_COUNT;
				dataitem->uwidth = measureRes.Width / HW_IMG;
				dataitem->uprewidth = 0;
				dataitem->upostwidth = 0;
				dataitem->vwidth = 1.0f / HW_COUNT;
				ckarrayData.push_back(dataitem);

				g.DrawString(inputWChar.c_str(), 1, &ft, drawPosition, &sf, &sb);
				++inputChar[0];
			}
		}
	}
}

void FontCraft::OnUnload() {
	// unregister event
	mEventOnLoad.UnregisterHandler();

	// gdi+
	if (m_core_props[0]->GetBoolean()) {
		delete bitmapFont;
		for (auto iter = ckarrayData.begin(); iter != ckarrayData.end(); iter++)
			delete (*iter);
		ckarrayData.clear();
		GdiplusShutdown(this->gdiplusToken);
	}
}

void FontCraft::OnLoadObject(YYCBML_CKSTRING filename, YYCBML_BOOL isMap, YYCBML_CKSTRING masterName,
	CK_CLASSID filterClass, YYCBML_BOOL addtoscene, YYCBML_BOOL reuseMeshes, YYCBML_BOOL reuseMaterials,
	YYCBML_BOOL dynamic, XObjectArray* objArray, CKObject* masterObj) {

	// process menu.nmo
	if (strcmp(filename, "3D Entities\\Menu.nmo")) return;

	if (m_core_props[0]->GetBoolean()) {
		CKContext* ctx = YYCBML_VISITOR->GetCKContext();
		
		// process image
		GetLogger()->Info("Processing Font_1...");
		CKTexture* font = (CKTexture*)ctx->GetObjectByName(YYCBML_TOCKSTRING("Font_1"));
		Gdiplus::BitmapData gdiimage;
		CKBYTE* ckimage = NULL;
		Gdiplus::Rect lockTerrtory(0, 0, HW_IMG, HW_IMG);
		DWORD* pCK = NULL;
		DWORD* pGDI = NULL;
		if ((bitmapFont->LockBits(&lockTerrtory, Gdiplus::ImageLockMode::ImageLockModeRead | Gdiplus::ImageLockMode::ImageLockModeWrite, PixelFormat32bppARGB, &gdiimage) == Gdiplus::Status::Ok) &&
			((ckimage = font->LockSurfacePtr()) != NULL)) {

			for (int rows = 0; rows < HW_IMG; ++rows) {
				pCK = (DWORD*)(ckimage + 4 * HW_IMG * rows);
				pGDI = (DWORD*)((BYTE*)gdiimage.Scan0 + (gdiimage.Stride * rows));
				for (int cols = 0; cols < HW_IMG; ++cols, ++pCK, ++pGDI) {
					*pCK = *pGDI;
				}
			}

			bitmapFont->UnlockBits(&gdiimage);
			font->ReleaseSurfacePtr();
			//font->FreeVideoMemory();
		} else GetLogger()->Error("Fail to lock GDI+ image.");

		// process ck array
		GetLogger()->Info("Processing M_FontData_01...");
		CKDataArray* dataarray = (CKDataArray*)ctx->GetObjectByName(YYCBML_TOCKSTRING("M_FontData_01"));
		dataarray->Clear();
		int counter = 0;
		for (auto iter = ckarrayData.begin(); iter != ckarrayData.end(); counter++, iter++) {
			dataarray->AddRow();
			dataarray->SetElementValue(counter, 0, &(*iter)->ustart, sizeof(float));
			dataarray->SetElementValue(counter, 1, &(*iter)->vstart, sizeof(float));
			dataarray->SetElementValue(counter, 2, &(*iter)->uwidth, sizeof(float));
			dataarray->SetElementValue(counter, 3, &(*iter)->uprewidth, sizeof(float));
			dataarray->SetElementValue(counter, 4, &(*iter)->upostwidth, sizeof(float));
			dataarray->SetElementValue(counter, 5, &(*iter)->vwidth, sizeof(float));
		}
	}
}

