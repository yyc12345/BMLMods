#include "Object3dManager.h"
#include "MeshCreator.h"
#include "MatrixBuilder.h"
#include <array>

namespace NSBallanceBBOR {

	Object3dManager::Object3dManager(CKContext* ctx, ILogger* logger, StructDisplayCfg* cfgmgr) :
		mCtx(ctx), mLogger(logger), mStructCfgManager(cfgmgr),
		mColorPalette(),
		mTransformerMesh(nullptr), mTransformerObjs(),
		mVentilatorActiveAreaMesh(nullptr), mVentilatorDeactiveAreaMesh(nullptr), mVentilatorActiveAreaObjs(), mVentilatorDeactiveAreaObjs() {

		// sync settings to self first to create all color platte
		SyncColors();

		// create matrix builder
		MatrixBuilder builder;
		// create transform mesh
		mTransformerMesh = static_cast<CKMesh*>(mCtx->CreateObject(CKCID_MESH));
		MeshCreator::CreateSphere(
			mTransformerMesh,
			PickColorPalette(BallanceStructType::Transformer),
			16, 8, 2.3f
		);
		// create ventilator active area mesh
		mVentilatorActiveAreaMesh = static_cast<CKMesh*>(mCtx->CreateObject(CKCID_MESH));
		MeshCreator::CreateCube(
			mVentilatorActiveAreaMesh,
			PickColorPalette(BallanceStructType::VentilatorActiveArea),
			1.0f
		);
		MeshCreator::TransformMesh(
			mVentilatorActiveAreaMesh,
			builder
			.Scale(2, 16, 2).Move(0, 8, 0)	// resize to its mesh size
			.Move(0, 1.1399f, -0.000588f).RotateWithEulerAngles(0.150428f, 0.000057f, 0)	// add `P_Modul_18_Kollisionsquader` extra offset.
			.Build()
		);
		// create ventilator deactive area mesh
		mVentilatorDeactiveAreaMesh = static_cast<CKMesh*>(mCtx->CreateObject(CKCID_MESH));
		MeshCreator::CreateCylinder(
			mVentilatorDeactiveAreaMesh,
			PickColorPalette(BallanceStructType::VentilatorDeactiveArea),
			16, 7.0f - 5.0f, 50.0f, true
		);

		CKFile* fs = mCtx->CreateCKFile();
		fs->StartSave("test.nmo");
		fs->SaveObject(mVentilatorDeactiveAreaMesh);
		fs->EndSave();
		mCtx->DeleteCKFile(fs);

	}

	Object3dManager::~Object3dManager() {
		// clear all data forcely
		ExitLevel();
	}

	void Object3dManager::EnterLevel() {
		// create all objects
		CreateTransformerList();
		CreateVentilatorList();
		// set enabled because object is displayed in default.
		SyncEnabled();
	}

	void Object3dManager::ExitLevel() {
		// clear all objects
		DestroyTransformerList();
		DestroyVentilatorList();
	}

	void Object3dManager::SyncSettings() {
		// both color and enable sync
		SyncColors();
		SyncEnabled();
	}

	void Object3dManager::SyncColors() {
		// sync color platte
		UpdateColorPalette(BallanceStructType::Transformer);
		UpdateColorPalette(BallanceStructType::VentilatorActiveArea);
		UpdateColorPalette(BallanceStructType::VentilatorDeactiveArea);
	}

	void Object3dManager::SyncEnabled() {
		// sync enable status
		SetObjectListVisibility(mTransformerObjs, BallanceStructType::Transformer);
		SetObjectListVisibility(mVentilatorActiveAreaObjs, BallanceStructType::VentilatorActiveArea);
		SetObjectListVisibility(mVentilatorDeactiveAreaObjs, BallanceStructType::VentilatorDeactiveArea);
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
		mtl->SetEmissive(color);
		// set alpha if needed
		if (color.a != 1.0f) {
			mtl->SetTwoSided(TRUE);

			// Copy from SpiritTrial
			mtl->EnableAlphaBlend();
			mtl->SetSourceBlend(VXBLEND_SRCALPHA);
			mtl->SetDestBlend(VXBLEND_INVSRCALPHA);

			// Copy from LaternFader
			mtl->EnableAlphaTest();
			mtl->SetAlphaFunc(VXCMP_GREATEREQUAL);
			mtl->SetAlphaRef(0);
		} else {
			mtl->SetTwoSided(FALSE);
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
			is_visible = cfg->IsEnabled();
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

	void Object3dManager::CreateVentilatorList() {
		DestroyTransformerList();
		CKScene* active_scene = mCtx->GetCurrentLevel()->GetLevelScene();

		// get by group
		CKGroup* ckgp = static_cast<CKGroup*>(mCtx->GetObjectByNameAndClass("P_Modul_18", CKCID_GROUP));
		if (ckgp == nullptr) return;
		int count = ckgp->GetObjectCount();
		for (int i = 0; i < count; ++i) {
			// try get world matrix
			CKBeObject* beobj = ckgp->GetObjectA(i);
			if (!CKIsChildClassOf(beobj, CKCID_3DENTITY)) continue;
			VxMatrix world_matrix(static_cast<CK3dEntity*>(beobj)->GetWorldMatrix());
			VxVector world_position;
			static_cast<CK3dEntity*>(beobj)->GetPosition(&world_position);

			// create new one and apply
			CK3dObject* active_showcase = static_cast<CK3dObject*>(mCtx->CreateObject(CKCID_3DOBJECT));
			active_showcase->SetCurrentMesh(mVentilatorActiveAreaMesh);
			active_showcase->SetWorldMatrix(world_matrix);
			active_scene->AddObjectToScene(active_showcase);
			// add into list
			mVentilatorActiveAreaObjs.emplace_back(active_showcase);

			// create new one and apply
			CK3dObject* deactive_showcase = static_cast<CK3dObject*>(mCtx->CreateObject(CKCID_3DOBJECT));
			deactive_showcase->SetCurrentMesh(mVentilatorDeactiveAreaMesh);
			deactive_showcase->SetPosition(world_position);
			active_scene->AddObjectToScene(deactive_showcase);
			// add into list
			mVentilatorDeactiveAreaObjs.emplace_back(deactive_showcase);
		}
	}

	void Object3dManager::DestroyVentilatorList() {
		for (auto* obj : mVentilatorActiveAreaObjs) {
			mCtx->DestroyObject(obj);
		}
		mVentilatorActiveAreaObjs.clear();

		for (auto* obj : mVentilatorDeactiveAreaObjs) {
			mCtx->DestroyObject(obj);
		}
		mVentilatorDeactiveAreaObjs.clear();
	}

}
