#pragma once

#include <YYCHelper.h>
#include "StructDisplayCfg.h"
#include <map>

namespace NSBallanceBBOR {

	class Object3dManager {
	public:
		Object3dManager(CKContext* ctx, ILogger* logger, StructDisplayCfg* cfgmgr);
		~Object3dManager();

		void EnterLevel();
		void ExitLevel();
		void SyncSettings();

	private:
		// ===== Basic Member =====

		CKContext* mCtx;
		StructDisplayCfg* mStructCfgManager;
		ILogger* mLogger;

		void SyncColors();
		void SyncEnabled();
		
		// ===== Color Material =====

		void UpdateColorPalette(BallanceStructType btype);
		CKMaterial* PickColorPalette(BallanceStructType btype);
		std::map<BallanceStructType, CKMaterial*> mColorPalette;
		
		// ===== 3dObject Manager =====

		void SetObjectListVisibility(std::vector<CK3dObject*>& ls, BallanceStructType btype);

		// Transformer

		void CreateTransformerList();
		void DestroyTransformerList();
		CKMesh* mTransformerMesh;
		std::vector<CK3dObject*> mTransformerObjs;

		// Ventilator
		void CreateVentilatorList();
		void DestroyVentilatorList();
		CKMesh* mVentilatorActiveAreaMesh, *mVentilatorDeactiveAreaMesh;
		std::vector<CK3dObject*> mVentilatorActiveAreaObjs, mVentilatorDeactiveAreaObjs;


	};

}
