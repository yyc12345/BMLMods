#include "BallanceBBOR.h"
#include <YYCHelper.h>

IMod* BMLEntry(IBML* bml) {
	return new BallanceBBOR(bml);
}
