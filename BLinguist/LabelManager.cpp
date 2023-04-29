#include "LabelManager.h"
#include <YYCHelper.h>
#include <array>
#include <sstream>

namespace NSBLinguist::LabelManager {

	BBTextDisplay::BBTextDisplay(CKContext* ctx) : ctx(ctx), tt(nullptr) {
		tt = (CKSpriteText*)ctx->CreateObject(CKCID_SPRITETEXT, "TextDisplay Sprite", CK_OBJECTCREATION_NONAMECHECK);
		tt->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
		tt->EnableClipToCamera(FALSE);
		tt->SetAlign(CKSPRITETEXT_ALIGNMENT(CKSPRITETEXT_HCENTER));

		static const Vx2DVector sg_DefaultSpriteSize(320.0f, 32.0f);
		tt->ReleaseAllSlots();
		tt->Create((int)sg_DefaultSpriteSize.x, (int)sg_DefaultSpriteSize.y, 32);

		// ARGB format
		static const CKDWORD sg_TextColor = 0xFFFFFFFF;
		tt->SetTextColor(sg_TextColor);

		tt->SetZOrder(100);

		ctx->GetCurrentLevel()->AddObject(tt);
		//ctx->GetCurrentScene()->AddObjectToScene(tt);
	}

	BBTextDisplay::~BBTextDisplay() {
		ctx->DestroyObject(tt);
	}

	void BBTextDisplay::SetVisible(bool is_visible) {
		this->tt->Show(is_visible ? CKSHOW : CKHIDE);
	}

	void BBTextDisplay::SetText(const std::string& text) {
		// set text with encoding convertion
		std::string native_str;
		if (!YYCHelper::EncodingHelper::CharToChar(text, native_str, CP_UTF8, CP_ACP)) {
			// fallback, use utf8 anywway
			this->tt->SetText(text.c_str());
		} else {
			// success.
			this->tt->SetText(native_str.c_str());
		}
	}

	void BBTextDisplay::SetFont(CKSTRING fontname, int fontsize) {
		this->tt->SetFont(fontname, fontsize, 400, 0, 0);
	}

	void BBTextDisplay::SetPosition(const Vx2DVector& pos) {
		this->tt->SetPosition(pos);
	}


	OffsetCK2dEntity::OffsetCK2dEntity(CKContext* ctx,
		const std::string& watching, const std::string& this_entity,
		const Vx2DVector& pos, const Vx2DVector& size) :
		mCtx(ctx), mWatching(nullptr), mCreated(nullptr) {
		// create new entity
		mCreated = (CK2dEntity*)mCtx->CreateObject(CKCID_2DENTITY, this_entity.c_str(), CK_OBJECTCREATION_NONAMECHECK);
		ctx->GetCurrentLevel()->AddObject(mCreated);

		// try getting ref entity
		CKObject* obj = mCtx->GetObjectByNameAndClass(watching.c_str(), CKCID_2DENTITY, NULL);
		if (obj != nullptr && obj->GetClassID() == CKCID_2DENTITY) {
			mWatching = (CK2dEntity*)obj;
		}
		if (mWatching) {
			mCreated->SetParent(mWatching);
		}

		// setup value
		mCreated->SetHomogeneousCoordinates(TRUE);
		mCreated->EnableClipToCamera(TRUE);

		mCreated->SetPosition(pos, TRUE);
		mCreated->SetSize(size, TRUE);

		mCreated->SetZOrder(-1000);

	}
	OffsetCK2dEntity::~OffsetCK2dEntity() {
		mCtx->DestroyObject(mCreated);
	}
	void OffsetCK2dEntity::Process(void) {
		if (mWatching != nullptr) {
			mCreated->Show(mWatching->IsVisible() ? CKSHOW : CKHIDE);
		}
	}


	LabelUI::LabelUI(CKContext* ctx,
		const std::string& watching, const std::string& text,
		const std::string& fontname, const int fontsize) :

		mOper(ctx), mCtx(ctx),
		mWatchingEntity(nullptr) {

		// get watching entity
		CKObject* obj = mCtx->GetObjectByNameAndClass(watching.c_str(), CKCID_2DENTITY, NULL);
		if (obj != nullptr && obj->GetClassID() == CKCID_2DENTITY) {
			mWatchingEntity = (CK2dEntity*)obj;
		}

		// set label
		mOper.SetVisible(false);
		mOper.SetFont(fontname.c_str(), fontsize);
		mOper.SetText(text);
	}

	LabelUI::~LabelUI() {
		mCtx = nullptr;
		mWatchingEntity = nullptr;
	}

	void LabelUI::Process(void) {
		if (mWatchingEntity == nullptr || !mWatchingEntity->IsVisible()) {
			// do not existed or invisible
			mOper.SetVisible(false);
		} else {
			// update location
			static const Vx2DVector offset(-160.0f, -7.5f);
			Vx2DVector size, pos;
			mWatchingEntity->GetSize(size);
			mWatchingEntity->GetPosition(pos);
			pos += size * 0.5f;
			pos += offset;

			mOper.SetPosition(pos);

			// show it
			mOper.SetVisible(true);
		}
	}


	LabelTutorial::LabelTutorial(CKContext* ctx, const std::string& text,
		const std::string& fontname, const int fontsize) :
		mCtx(ctx), mLines(), mFontSize(fontsize) {

		// split text by line
		std::stringstream ss;
		while (true) {
			std::string linetext;
			if (!std::getline(ss, linetext, '\n')) break;

			// create new line
			BBTextDisplay* newline = new BBTextDisplay(ctx);
			newline->SetVisible(false);
			newline->SetFont(fontname.c_str(), fontsize);
			newline->SetText(linetext);

			// insert it
			mLines.emplace_back(newline);
		}
	}
	LabelTutorial::~LabelTutorial() {
		for (const auto& item : mLines) {
			delete (item);
		}
		mLines.clear();
	}
	void LabelTutorial::SetVisible(bool is_visible) {
		for (const auto& item : mLines) {
			item->SetVisible(is_visible);
		}
	}
	void LabelTutorial::Process(const Vx2DVector& tut_pos) {
		int line = 0;
		for (const auto& item : mLines) {
			Vx2DVector relativePos(0.0f, line * mFontSize * (25.0f / 10.0f) + 7.5f);
			item->SetPosition(relativePos + tut_pos);
			++line;
		}
	}


	static const std::string g_TutorialInterfaceName = "Tutorial_Interface";
	// Tuple: watching_entity, created_entity, pos.x, pox.y, size.x, size.y
	static const std::array<std::tuple<std::string, std::string, float, float, float, float>, 5u> g_OffsetCK2dEntities{
		std::make_tuple("M_Opt_Gra_CloudsField", "M_Opt_Gra_CloudsField2", 0.2029f, 0.2025f, 0.7250f, 0.2405f),
		std::make_tuple("M_Opt_Gra_ResField", "M_Opt_Gra_ResField2", 0.1780f, 0.1934f, 0.7579f, 0.3038f),
		std::make_tuple("M_Opt_Graph_SynchField", "M_Opt_Graph_SynchField2", 0.1931f, 0.1760f, 0.7262f, 0.2785f),
		std::make_tuple("M_Opt_Keys_Inv_Field", "M_Opt_Keys_Inv_Field2", 0.1591f, 0.1855f, 0.7716f, 0.3654f),
		std::make_tuple("M_Opt_Sound_VolField", "M_Opt_Sound_VolField2", 0.1966f, 0.1837f, 0.7254f, 0.3430f)
	};
	static const std::array<std::string, 70u> g_UIWatchingEntities{
		"M_Main_But_5",
		"M_Main_But_4",
		"M_Main_But_3",
		"M_Main_But_2",
		"M_Main_But_1",
		"M_Start_But_Back",
		"M_Start_But_10",
		"M_Start_But_09",
		"M_Start_But_08",
		"M_Start_But_07",
		"M_Start_But_06",
		"M_Start_But_05",
		"M_Start_But_04",
		"M_Start_But_03",
		"M_Start_But_02",
		"M_Start_But_01",
		"M_Pause_But_Back",
		"M_Pause_But_4",
		"M_Pause_But_3",
		"M_Pause_But_2",
		"M_Pause_But_1",
		"M_YesNo_TextSprite",
		"M_YesNo_But_Yes",
		"M_YesNo_But_No",
		"M_Dead_But_2",
		"M_Dead_But_1",
		"M_End_But_5",
		"M_End_But_4",
		"M_End_But_3",
		"M_End_But_2",
		"M_End_But_1",
		"M_Credits_But_Back",
		"M_Highscore_But_Back",
		"M_Options_But_Back",
		"M_HighEntry_But_1",
		"M_HighEntry_Title",
		"M_Highscore_Title",
		"M_Options_But_1",
		"M_Options_But_2",
		"M_Options_But_3",
		"M_Options_Title",
		"M_Opt_Graph_Title",
		"M_Opt_Gra_ResField2",
		"M_Opt_Gra_Back",
		"M_Opt_Gra_SynchYes",
		"M_Opt_Graph_SynchField2",
		"M_Opt_Gra_SynchNo",
		"M_Opt_Gra_CloudsYes",
		"M_Opt_Gra_CloudsNo",
		"M_Opt_Gra_CloudsField2",
		"M_Opt_Keys_Title",
		"M_Opt_Keys_Inv_Yes",
		"M_Opt_Keys_Inv_No",
		"M_Opt_Keys_Inv_Field2",
		"M_Opt_Keys_But_Back",
		"M_Opt_Keys_Key6",
		"M_Opt_Keys_Key4",
		"M_Opt_Keys_Key3",
		"M_Opt_Keys_Key2",
		"M_Opt_Keys_Key1",
		"M_Opt_Keys_Key5",
		"M_Opt_Sound_Title",
		"M_Opt_Sound_But_Back",
		"M_Opt_Sound_VolField2",
		"M_Score_Text4",
		"M_Score_Text3",
		"M_Score_Text2",
		"M_Score_Text1",
		"M_Start_But_12",
		"M_Start_But_11"
	};
	LabelsCollection::LabelsCollection(CKContext* ctx,
		const LangManager::UITrCollection& ui, const LangManager::TutorialTrCollection& tutorial,
		const std::string& fontname, const int fontsize) :

		mUIList(), mTutorialList(), mOffsetEntities(), mCtx(ctx),
		mUICounter(0), mTutIdRef(nullptr) {

		// init offset 2d entities
		for (const auto& item : g_OffsetCK2dEntities) {
			this->mOffsetEntities.emplace_back(new OffsetCK2dEntity(
				mCtx, std::get<0>(item), std::get<1>(item),
				Vx2DVector(std::get<2>(item), std::get<3>(item)), Vx2DVector(std::get<4>(item), std::get<5>(item))
			));
		}

		// init ui labels
		for (int i = 0; i < ui.size(); ++i) {
			this->mUIList.emplace_back(new LabelUI(mCtx, g_UIWatchingEntities[i], ui[i], fontname, fontsize));
		}
		// init tutorial labels
		for (const auto& tut : tutorial) {
			this->mTutorialList.emplace_back(new LabelTutorial(mCtx, tut, fontname, fontsize));
		}
	}

	LabelsCollection::~LabelsCollection() {
		for (const auto& item : mUIList) {
			delete (item);
		}
		mUIList.clear();

		for (const auto& item : mTutorialList) {
			delete (item);
		}
		mTutorialList.clear();

		for (const auto& item : mOffsetEntities) {
			delete (item);
		}
		mOffsetEntities.clear();

	}

	void LabelsCollection::SetTutorialIDRef(CKParameterLocal* plocal) {
		mTutIdRef = plocal;
	}

	void LabelsCollection::Process(void) {
		// execute UI check per 6 frames
		++mUICounter;
		if (mUICounter >= 6) {
			mUICounter = 0;

			for (auto& item : mOffsetEntities) {
				item->Process();
			}
			for (auto& item : mUIList) {
				item->Process();
			}

			CKObject* obj = mCtx->GetObjectByNameAndClass(g_TutorialInterfaceName.c_str(), CKCID_2DENTITY, NULL);
			if (obj != nullptr && obj->GetClassID() == CKCID_2DENTITY) {
				CK2dEntity* tut = (CK2dEntity*)obj;
				Vx2DVector tutpos;
				tut->GetPosition(tutpos);
				int tutid = *(int*)mTutIdRef->GetReadDataPtr(FALSE);

				for (int i = 0; i < mTutorialList.size(); ++i) {
					LabelTutorial* item = mTutorialList[i];
					item->Process(tutpos);
					item->SetVisible(i == tutid);
				}
			} else {
				for (auto& item : mTutorialList) {
					item->SetVisible(false);
				}
			}
		}
	}


}
