#include "MatrixBuilder.h"
#include <numbers>

namespace NSBallanceBBOR {

	MatrixBuilder::MatrixBuilder() : mMat() {
		mMat.SetIdentity();
	}

	MatrixBuilder::~MatrixBuilder() {}

	float MatrixBuilder::Degree2Rad(float degree) {
		constexpr float percent = (1.0f / 360.0f) * std::numbers::pi_v<float> * 2.0f;
		return degree * percent;
	}

	MatrixBuilder& MatrixBuilder::Reset() {
		mMat.SetIdentity();
		return *this;
	}

	MatrixBuilder& MatrixBuilder::Move(float x, float y, float z) {
		/*
				[ 1				0			0			0]
				[ 0				1			0			0]
		  MAT =	[ 0				0			1			0]
				[ x				y			z			1]
		*/
		VxMatrix cache(VxMatrix::Identity());
		cache[3][0] = x;
		cache[3][1] = y;
		cache[3][2] = z;
		mMat *= cache;

		return *this;
	}

	MatrixBuilder& MatrixBuilder::RotateWithAxisAngle(const VxVector& axis, float angle) {
		VxMatrix cache;
		Vx3DMatrixFromRotation(cache, axis, Degree2Rad(angle));
		mMat *= cache;

		return *this;
	}

	MatrixBuilder& MatrixBuilder::RotateWithEulerAngles(float x, float y, float z) {
		VxMatrix cache;
		Vx3DMatrixFromEulerAngles(cache, Degree2Rad(x), Degree2Rad(y), Degree2Rad(y));
		mMat *= cache;

		return *this;
	}

	MatrixBuilder& MatrixBuilder::Scale(float x, float y, float z) {
		/*
				[ x				0			0			0]
				[ 0				y			0			0]
		  MAT =	[ 0				0			z			0]
				[ 0				0			0			1]
		*/
		VxMatrix cache(VxMatrix::Identity());
		cache[0][0] = x;
		cache[1][1] = y;
		cache[2][2] = z;
		mMat *= cache;

		return *this;
	}

	VxMatrix MatrixBuilder::Build() {
		VxMatrix ret(mMat);
		mMat.Identity();
		return ret;
	}

}
