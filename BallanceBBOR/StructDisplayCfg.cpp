#include "StructDisplayCfg.h"

namespace NSBallanceBBOR {

	StructDisplayCfgItem::StructDisplayCfgItem(
		IConfig* cfgmgr,
		CKSTRING category_name, CKSTRING category_desc,
		bool default_enabled, CKSTRING default_color) :
		mPropEnabled(nullptr), mPropDisplayColor(nullptr) {

		// register property category
		cfgmgr->SetCategoryComment(category_name, category_desc);

		// register properties
		mPropEnabled = cfgmgr->GetProperty(category_name, "Enabled");
		mPropEnabled->SetComment("The enable switch for this struct.");
		mPropEnabled->SetDefaultBoolean(default_enabled);

		mPropDisplayColor = cfgmgr->GetProperty(category_name, "Display Color");
		mPropDisplayColor->SetComment("The display color of this struct. Support both #rrggbbaa and rrr,ggg,bbb,aaa format. Alpha factor is optional. First is Hex style and second is Dec style.");
		mPropDisplayColor->SetDefaultString(default_color);
	}

	StructDisplayCfgItem::~StructDisplayCfgItem() {}

	bool StructDisplayCfgItem::IsEnabled() {
		return mPropEnabled->GetBoolean();
	}

	VxColor StructDisplayCfgItem::GetColor() {
		// default color is gray and opaque.
		static VxColor default_color(127, 127, 127, 255);

		VxColor ret;
		if (!YYCHelper::ColorStringParser::ParseColorString(mPropDisplayColor->GetString(), ret)) {
			// if failed, set by default color
			ret = default_color;
		}

		return ret;
	}


	StructDisplayCfg::StructDisplayCfg(IConfig* cfgmgr) : mStructCfgs(), mCfgMgr(cfgmgr), mPropGlobalEnabled(nullptr) {
		// Register global options first
		mCfgMgr->SetCategoryComment("Global", "The global switch of this mod.");
		mPropGlobalEnabled = mCfgMgr->GetProperty("Global", "Enabled");
		mPropGlobalEnabled->SetComment("The enable switch for global scope.");
		mPropGlobalEnabled->SetDefaultBoolean(false);

		// then register other options
		RegisterProps();
	}

	StructDisplayCfg::~StructDisplayCfg() {}

	bool StructDisplayCfg::IsGlobalEnabled() {
		return mPropGlobalEnabled->GetBoolean();
	}

	void StructDisplayCfg::RegisterProps() {
		mStructCfgs.emplace(BallanceStructType::DepthCubes, StructDisplayCfgItem(mCfgMgr, "DepthCubes", "The area that ball lost when touching.", true, "#ff0000"));
		mStructCfgs.emplace(BallanceStructType::Transformer, StructDisplayCfgItem(mCfgMgr, "Transformer", "The transformer active attracting area.", true, "#ff7f00"));
		mStructCfgs.emplace(BallanceStructType::VentilatorActiveArea, StructDisplayCfgItem(mCfgMgr, "VentilatorActiveArea", "The area that ventilator can add force to ball.", true, "#007fff"));
		mStructCfgs.emplace(BallanceStructType::VentilatorDeactiveArea, StructDisplayCfgItem(mCfgMgr, "VentilatorDeactiveArea", "The plane that ventilator retreat added force.", false, "#0000ff7f"));
		mStructCfgs.emplace(BallanceStructType::PhysHiddenFloor, StructDisplayCfgItem(mCfgMgr, "PhysHiddenFloor", "The hidden object physicalized as floor.", true, "#ffff00"));
		mStructCfgs.emplace(BallanceStructType::PhysHiddenStopper, StructDisplayCfgItem(mCfgMgr, "PhysHiddenStopper", "The hidden object physicalized as stopper.", false, "#00ff00"));
	}

	StructDisplayCfgItem* StructDisplayCfg::GetCfgItem(BallanceStructType cfg_type) {
		auto finder = mStructCfgs.find(cfg_type);
		if (finder == mStructCfgs.end()) return nullptr;
		else return &finder->second;
	}

}
