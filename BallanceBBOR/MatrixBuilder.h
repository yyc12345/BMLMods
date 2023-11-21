#pragma once

#include <YYCHelper.h>

namespace NSBallanceBBOR {

	class MatrixBuilder {
	public:
		MatrixBuilder();
		~MatrixBuilder();

		static float Degree2Rad(float degree);
		MatrixBuilder& Reset();
		MatrixBuilder& Set(const VxMatrix& mat);
		MatrixBuilder& Move(float x, float y, float z);
		MatrixBuilder& RotateWithAxisAngle(const VxVector& axis, float angle);
		MatrixBuilder& RotateWithEulerAngles(float x, float y, float z);
		MatrixBuilder& Scale(float x, float y, float z);
		MatrixBuilder& Inverse();
		MatrixBuilder& Transpose();
		VxMatrix Build();

	private:
		VxMatrix mMat;
	};

}