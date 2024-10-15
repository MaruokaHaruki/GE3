///=====================================================/// 
///3x3行列演算
///=====================================================///

#pragma once
#include "Vector3.h"
#include "Matrix3x3.h"
#include <cmath>

	//内積
inline float Dot(const Vector3& v1, const Vector3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// ベクトルの大きさ（長さ）を計算する関数
inline float Magnitude(const Vector3& v) {
	return std::sqrt(Dot(v, v));
}

// ベクトルを正規化
inline Vector3 Normalize(const Vector3& v) {
	float mag = Magnitude(v);
	if (mag != 0.0f) {
		return { v.x / mag, v.y / mag, v.z / mag };
	}
	// ゼロベクトルの場合はそのまま返す
	return v;
}
