#pragma once

#include <YYCHelper.h>
#include <map>

namespace NSBallanceBBOR {

	enum class BallanceStructType : uint32_t {
		DepthCubes,
		Transformer,
		VentilatorActiveArea,
		VentilatorDeactivePlane,
		PhysHiddenFloor,
		PhysHiddenStopper
	};

	class StructDisplayCfgItem {
	public:
		StructDisplayCfgItem(
			IConfig* cfgmgr, 
			CKSTRING category_name, CKSTRING category_desc,
			bool default_enabled, CKSTRING default_color);
		~StructDisplayCfgItem();

		bool IsEnabled();
		VxColor GetColor();

	private:
		IProperty* mPropEnabled;
		IProperty* mPropDisplayColor;
	};

	class StructDisplayCfg {
	public:
		StructDisplayCfg(IConfig* cfgmgr);
		~StructDisplayCfg();

		void RegisterProps();
		StructDisplayCfgItem* GetCfgItem(BallanceStructType cfg_type);

	private:
		IConfig* mCfgMgr;
		std::map<BallanceStructType, StructDisplayCfgItem> mStructCfgs;
	};

}
