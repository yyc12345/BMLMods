#include "YYCHelper.h"

namespace YYCHelper::BMLPatches {

#if defined(YYCMOD_BMLP_USED)
	/**
	 * A fake class wrapper to expose interface.
	*/
	class CKBehaviorIOWrapper : public CKBehaviorIO {
	public:
		XSObjectPointerArray* GetLinks() {
			return &this->m_Links;
		}
	};
#endif

	XSObjectPointerArray* CKBehaviorIO_GetLinks(CKBehaviorIO* that) {
#if defined(YYCMOD_BML_USED)
		return that->GetLinks();
#else
		auto* wrapperBBCreateString_CreatOut = reinterpret_cast<YYCHelper::BMLPlusPatch::CKBehaviorIOWrapper*>(that);
		return wrapperBBCreateString_CreatOut->GetLinks();
#endif
	}


	static CKDWORD VxColor_RGBAFTOCOLOR(const VxColor* col) {
#if defined(YYCMOD_BML_USED)
		// polyfill for BML
		CKDWORD intcol = 0x0;
		intcol |= (static_cast<CKDWORD>(col->a * 255.0f) << A_SHIFT) & A_MASK;
		intcol |= (static_cast<CKDWORD>(col->r * 255.0f) << R_SHIFT) & R_MASK;
		intcol |= (static_cast<CKDWORD>(col->g * 255.0f) << G_SHIFT) & G_MASK;
		intcol |= (static_cast<CKDWORD>(col->b * 255.0f) << B_SHIFT) & B_MASK;
		return intcol;
#else
		// call directly
		static_cast<CKDWORD>(RGBAFTOCOLOR(col));
#endif
	}
	CKDWORD VxColor_GetRGB(const VxColor& that) {
#if defined(YYCMOD_BML_USED)
		// polyfill for BML
		return VxColor_RGBAFTOCOLOR(&that);
#else
		// call directly
		static_cast<CKDWORD>(that.GetRGB());
#endif
	}
	CKDWORD VxColor_GetRGBA(const VxColor& that) {
#if defined(YYCMOD_BML_USED)
		// polyfill for BML
		return VxColor_RGBAFTOCOLOR(&that) | A_MASK;
#else
		// call directly
		static_cast<CKDWORD>(that.GetRGBA());
#endif
	}

}
