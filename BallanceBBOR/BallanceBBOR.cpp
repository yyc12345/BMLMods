#include "BallanceBBOR.h"
#include <YYCHelper.h>

IMod* BMLEntry(IBML* bml) {
	return new BallanceBBOR(bml);
}

void BallanceBBOR::OnLoad() {
	mStructCfgManager = new NSBallanceBBOR::StructDisplayCfg(GetConfig());
	mObjectsManager = new NSBallanceBBOR::Object3dManager(YYCBML_VISITOR->GetCKContext(), GetLogger(), mStructCfgManager);
}

void BallanceBBOR::OnUnload() {
	if (mObjectsManager) {
		delete mObjectsManager;
		mObjectsManager = nullptr;
	}
	if (mStructCfgManager) {
		delete mStructCfgManager;
		mStructCfgManager = nullptr;
	}
}

//void BallanceBBOR::OnPreLoadLevel() {
//	if (mObjectsManager) {
//		mObjectsManager->EnterLevel();
//	}
//}

void BallanceBBOR::OnPreExitLevel() {
	if (mObjectsManager) {
		mObjectsManager->ExitLevel();
	}
}

void BallanceBBOR::OnModifyConfig(YYCBML_CKSTRING category, YYCBML_CKSTRING key, IProperty* prop) {
	if (mObjectsManager) {
		mObjectsManager->SyncSettings();
	}
}

void BallanceBBOR::OnLoadObject(YYCBML_CKSTRING filename, YYCBML_BOOL isMap, YYCBML_CKSTRING masterName,
		CK_CLASSID filterClass, YYCBML_BOOL addtoscene, YYCBML_BOOL reuseMeshes, YYCBML_BOOL reuseMaterials,
		YYCBML_BOOL dynamic, XObjectArray* objArray, CKObject* masterObj) {
	// only proc map
	if (!isMap) return;

	if (mObjectsManager) {
		mObjectsManager->EnterLevel();
	}
}
