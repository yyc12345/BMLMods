#pragma once

#include <YYCHelper.h>
#include "StructDisplayCfg.h"
#include <map>

namespace NSBallanceBBOR {

	class Object3dManager {
	public:
		Object3dManager();
		~Object3dManager();

	private:
		std::map<BallanceStructType, CKMaterial*> mColorPalette;
	};

}
