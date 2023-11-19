#include "Object3dManager.h"
#include "MeshCreator.h"
#include <array>

namespace NSBallanceBBOR {

	Object3dManager::Object3dManager(CKContext* ctx, ILogger* logger, StructDisplayCfg* cfgmgr) :
		mCtx(ctx), mLogger(logger), mStructCfgManager(cfgmgr),
		mColorPalette(),
		mTransformerMesh(nullptr), mTransformerObjs()
	{
		// sync settings to self first to create all color platte 
		// (and sync enabled status, but it is vain)
		SyncSettings();

		// create mesh
		mTransformerMesh = static_cast<CKMesh*>(mCtx->CreateObject(CKCID_MESH));
		MeshCreator::CreateSphere(
			mTransformerMesh,
			PickColorPalette(BallanceStructType::Transformer),
			16, 8, 2.3f
		);

	}

	Object3dManager::~Object3dManager() {
		// clear all data forcely
		ExitLevel();
	}

	void Object3dManager::EnterLevel() {
		// create all objects
		CreateTransformerList();
	}

	void Object3dManager::ExitLevel() {
		// clear all objects
		DestroyTransformerList();
	}

	void Object3dManager::SyncSettings() {
		// sync color platte
		UpdateColorPalette(BallanceStructType::Transformer);

		// sync enable status
		SetObjectListVisibility(mTransformerObjs, BallanceStructType::Transformer);
	}

	void Object3dManager::UpdateColorPalette(BallanceStructType btype) {
		// get cfg first
		StructDisplayCfgItem* cfg = mStructCfgManager->GetCfgItem(btype);
		if (cfg == nullptr) return;
		VxColor color = cfg->GetColor();

		// get or create material in self map
		CKMaterial* mtl = nullptr;
		auto finder = mColorPalette.find(btype);
		if (finder == mColorPalette.end()) {
			// no item. create one
			mtl = static_cast<CKMaterial*>(mCtx->CreateObject(CKCID_MATERIAL));
			mColorPalette.emplace(btype, mtl);
		} else {
			// get existing one
			mtl = finder->second;
		}

		// update mtl color
		mtl->SetDiffuse(color);
		// set alpha if needed
		if (color.a != 1.0f) {
			// Copy from SpiritTrial
			mtl->EnableAlphaBlend();
			mtl->SetSourceBlend(VXBLEND_SRCALPHA);
			mtl->SetDestBlend(VXBLEND_INVSRCALPHA);
			// Copy from LaternFader
			mtl->EnableAlphaTest();
			mtl->SetAlphaFunc(VXCMP_GREATEREQUAL);
			mtl->SetAlphaRef(0);
		} else {
			mtl->EnableAlphaBlend(FALSE);
			mtl->EnableAlphaTest(FALSE);
		}
	}

	CKMaterial* Object3dManager::PickColorPalette(BallanceStructType btype) {
		auto finder = mColorPalette.find(btype);
		if (finder == mColorPalette.end()) return nullptr;
		else return finder->second;
	}

	void Object3dManager::SetObjectListVisibility(std::vector<CK3dObject*>& ls, BallanceStructType btype) {
		// get cfg first, considering global enabled.
		bool is_visible = false;
		if (mStructCfgManager->IsGlobalEnabled()) {
			StructDisplayCfgItem* cfg = mStructCfgManager->GetCfgItem(btype);
			if (cfg == nullptr) return;
			bool is_visible = cfg->IsEnabled();
		}

		// process list
		for (auto* item : ls) {
			if (is_visible) item->Show(CKSHOW);
			else item->Show(CKHIDE);
		}
	}

	void Object3dManager::CreateTransformerList() {
		DestroyTransformerList();
		CKScene* active_scene = mCtx->GetCurrentLevel()->GetLevelScene();

		// get by group
		static std::array<std::string, 3> s_GroupNames {
			"P_Trafo_Wood",
			"P_Trafo_Stone",
			"P_Trafo_Paper"
		};
		for (const auto& gp : s_GroupNames) {
			CKGroup* ckgp = static_cast<CKGroup*>(mCtx->GetObjectByNameAndClass(YYCBML_TOCKSTRING(gp.c_str()), CKCID_GROUP));
			if (ckgp == nullptr) continue;
			int count = ckgp->GetObjectCount();
			for (int i = 0; i < count; ++i) {
				// try get world matrix
				CKBeObject* beobj = ckgp->GetObjectA(i);
				if (!CKIsChildClassOf(beobj, CKCID_3DENTITY)) continue;
				VxMatrix world_matrix(static_cast<CK3dEntity*>(beobj)->GetWorldMatrix());
				
				// create new one and apply
				CK3dObject* showcase = static_cast<CK3dObject*>(mCtx->CreateObject(CKCID_3DOBJECT));
				showcase->SetCurrentMesh(mTransformerMesh);
				showcase->SetWorldMatrix(world_matrix);
				active_scene->AddObjectToScene(showcase);

				// add into list
				mTransformerObjs.emplace_back(showcase);
			}
		}
	}

	void Object3dManager::DestroyTransformerList() {
		for (auto* obj : mTransformerObjs) {
			mCtx->DestroyObject(obj);
		}
		mTransformerObjs.clear();
	}

}
