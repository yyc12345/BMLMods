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



	LabelsCollection::LabelsCollection(CKContext* ctx,
		const LangManager::UITrCollection& ui, const LangManager::TutorialTrCollection& tutorial,
		const std::string& fontname, const int fontsize) :

		mUIList(), mCtx(ctx),
		mUICounter(0) {

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
	}

	void LabelsCollection::Process(void) {
		// execute UI check per 6 frames
		++mUICounter;
		if (mUICounter >= 6) {
			mUICounter = 0;

			for (auto& item : mUIList) {
				item->Process();
			}
		}
	}


}
