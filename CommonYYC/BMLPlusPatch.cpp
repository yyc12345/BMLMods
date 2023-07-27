#include "YYCHelper.h"

namespace YYCHelper::BMLPlusPatch {

#if defined(YYCMOD_BMLP_USED)
	XSObjectPointerArray* YYCHelper::BMLPlusPatch::CKBehaviorIOWrapper::GetLinks()
	{
		return &this->m_Links;
	}
#endif

}
