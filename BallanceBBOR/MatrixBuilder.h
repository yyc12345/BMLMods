#pragma once

#include <YYCHelper.h>

namespace NSBallanceBBOR {

	class MatrixBuilder {
	public:
		MatrixBuilder();
		~MatrixBuilder();

		static float Degree2Rad(float degree);
		MatrixBuilder& Reset();
		MatrixBuilder& Move(float x, float y, float z);
		MatrixBuilder& RotateWithAxisAngle(const VxVector& axis, float angle);
		MatrixBuilder& RotateWithEulerAngles(float x, float y, float z);
		MatrixBuilder& Scale(float x, float y, float z);
		VxMatrix Build();

	private:
		VxMatrix mMat;
	};

}