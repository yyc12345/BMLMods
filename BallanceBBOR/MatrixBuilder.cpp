#include "MatrixBuilder.h"
#include <numbers>

namespace NSBallanceBBOR {

	static void CorrectMatrixMul(VxMatrix& ret, const VxMatrix& matA, const VxMatrix& matB) {
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				ret[i][j] = 0;
				for (int p = 0; p < 4; ++p) {
					ret[i][j] += matA[i][p] * matB[p][j];
				}
			}
		}
	}

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

	MatrixBuilder& MatrixBuilder::Set(const VxMatrix& mat) {
		mMat = mat;
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
		//mMat *= cache;
		VxMatrix temp(mMat);
		CorrectMatrixMul(mMat, temp, cache);

		return *this;
	}

	MatrixBuilder& MatrixBuilder::RotateWithAxisAngle(const VxVector& axis, float angle) {
		VxMatrix cache;
		Vx3DMatrixFromRotation(cache, axis, Degree2Rad(angle));
		//mMat *= cache;
		VxMatrix temp(mMat);
		CorrectMatrixMul(mMat, temp, cache);

		return *this;
	}

	MatrixBuilder& MatrixBuilder::RotateWithEulerAngles(float x, float y, float z) {
		VxMatrix cache;
		Vx3DMatrixFromEulerAngles(cache, Degree2Rad(x), Degree2Rad(y), Degree2Rad(z));
		//mMat *= cache;
		VxMatrix temp(mMat);
		CorrectMatrixMul(mMat, temp, cache);

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
		//mMat *= cache;
		VxMatrix temp(mMat);
		CorrectMatrixMul(mMat, temp, cache);

		return *this;
	}

	MatrixBuilder& MatrixBuilder::Inverse() {
		VxMatrix temp(mMat);
		Vx3DInverseMatrix(mMat, temp);
		return *this;
	}

	MatrixBuilder& MatrixBuilder::Transpose() {
		VxMatrix temp(mMat);
		Vx3DTransposeMatrix(mMat, temp);
		return *this;
	}

	VxMatrix MatrixBuilder::Build() {
		VxMatrix ret(mMat);
		mMat.SetIdentity();
		return ret;
	}

}
