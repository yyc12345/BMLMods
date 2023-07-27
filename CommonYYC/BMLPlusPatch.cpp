#include "YYCHelper.h"

namespace YYCHelper::BMLPlusPatch {

	XSObjectPointerArray* YYCHelper::BMLPlusPatch::CKBehaviorIOWrapper::GetLinks()
	{
		return &this->m_Links;
	}

}
