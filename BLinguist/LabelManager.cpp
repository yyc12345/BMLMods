#include "LabelManager.h"
#include <YYCHelper.h>

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

	void BBTextDisplay::SetText(CKSTRING strl) {
		this->tt->SetText(strl);
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

		// set text
		std::string native_str;
		if (!YYCHelper::EncodingHelper::CharToChar(text, native_str, CP_UTF8, CP_ACP)) {
			// fallback, use utf8 anywway
			mOper.SetText(text.c_str());
		} else {
			// success.
			mOper.SetText(native_str.c_str());
		}

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

	// Tuple: watching_entity, created_entity, pos.x, pox.y, size.x, size.y
	static const std::vector<std::tuple<std::string, std::string, float, float, float, float>> g_OffsetCK2dEntities{
		{"M_Opt_Gra_CloudsField", "M_Opt_Gra_CloudsField2", 0.2029f, 0.2025f, 0.7250f, 0.2405f},
		{"M_Opt_Gra_ResField", "M_Opt_Gra_ResField2", 0.1780f, 0.1934f, 0.7579f, 0.3038f},
		{"M_Opt_Graph_SynchField", "M_Opt_Graph_SynchField2", 0.1931f, 0.1760f, 0.7262f, 0.2785f},
		{"M_Opt_Keys_Inv_Field", "M_Opt_Keys_Inv_Field2", 0.1591f, 0.1855f, 0.7716f, 0.3654f},
		{"M_Opt_Sound_VolField", "M_Opt_Sound_VolField2", 0.1966f, 0.1837f, 0.7254f, 0.3430f}
	};
	LabelsCollection::LabelsCollection(CKContext* ctx,
		const LangManager::UITrCollection& ui, const LangManager::TutorialTrCollection& tutorial,
		const std::string& fontname, const int fontsize) :

		mUIList(), mOffsetEntities(), mCtx(ctx),
		mUICounter(0) {

		// init offset 2d entities
		for (const auto& item : g_OffsetCK2dEntities) {
			this->mOffsetEntities.emplace_back(new OffsetCK2dEntity(
				mCtx, std::get<0>(item), std::get<1>(item),
				Vx2DVector(std::get<2>(item), std::get<3>(item)), Vx2DVector(std::get<4>(item), std::get<5>(item))
			));
		}

		// init ui labels
		for (const auto& item : ui) {
			this->mUIList.emplace_back(new LabelUI(mCtx, item.first, item.second, fontname, fontsize));
		}
		// todo: init tutorial labels

	}

	LabelsCollection::~LabelsCollection() {
		for (const auto& item : mUIList) {
			delete (item);
		}
		mUIList.clear();

		for (const auto& item : mOffsetEntities) {
			delete (item);
		}
		mOffsetEntities.clear();

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
		}
	}


}
