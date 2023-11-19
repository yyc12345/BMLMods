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
		auto* wrapperBBCreateString_CreatOut = reinterpret_cast<CKBehaviorIOWrapper*>(that);
		return wrapperBBCreateString_CreatOut->GetLinks();
#endif
	}

}
