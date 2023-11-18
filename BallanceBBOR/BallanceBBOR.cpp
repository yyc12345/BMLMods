#include "BallanceBBOR.h"
#include <YYCHelper.h>

IMod* BMLEntry(IBML* bml) {
	return new BallanceBBOR(bml);
}

void BallanceBBOR::OnLoad() {

}

void BallanceBBOR::OnUnload() {

}

void BallanceBBOR::OnLoadObject(YYCBML_CKSTRING filename, YYCBML_BOOL isMap, YYCBML_CKSTRING masterName,
		CK_CLASSID filterClass, YYCBML_BOOL addtoscene, YYCBML_BOOL reuseMeshes, YYCBML_BOOL reuseMaterials,
		YYCBML_BOOL dynamic, XObjectArray* objArray, CKObject* masterObj) {

}
