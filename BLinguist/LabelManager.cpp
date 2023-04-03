#include "LabelManager.h"
#include <YYCHelper.h>

namespace NSBLinguist::LabelManager {

	LabelUI::LabelUI(CKContext* ctx,
		const std::string& watching, const std::string& text,
		const std::string& fontname, const int fontsize) :

		mOper("TextDisplay Sprite"), mCtx(ctx),
		mWatchingEntity(nullptr) {

		// get watching entity
		CKObject* obj = mCtx->GetObjectByNameAndClass(watching.c_str(), CKCID_2DENTITY, NULL);
		if (obj != nullptr && obj->GetClassID() == CKCID_2DENTITY) {
			mWatchingEntity = (CK2dEntity*)obj;
		}

		// set label
		mOper.SetVisible(false);
		mOper.SetAlignment(CKSPRITETEXT_ALIGNMENT(CKSPRITETEXT_HCENTER));
		mOper.SetFont(/*fontname.c_str()*/"Arial", fontsize, 400, false, false);

		// set text
		std::string native_str;
		if (!YYCHelper::EncodingHelper::CharToChar(text, native_str, CP_UTF8, CP_ACP)) {
			// fallback, use utf8 anywway
			mOper.SetText(text.c_str());
		}
		mOper.SetText(native_str.c_str());
		mOper.SetText("caonima");

		// hack BML to call set size
		struct FakeBGuiText {
			CKDWORD vtables;
			CK2dEntity* m_2dentity;
			CKSpriteText* m_sprite;
		};
		FakeBGuiText* ptr = reinterpret_cast<FakeBGuiText*>(&mOper);
		ptr->m_sprite->ReleaseAllSlots();
		ptr->m_sprite->Create(320, 32, 32);

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
