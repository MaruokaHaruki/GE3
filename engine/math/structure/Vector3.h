#pragma once

#include <cmath>

/// <summary>
/// 3次元ベクトル
/// </summary>
struct Vector3 final{
	float x;
	float y;
	float z;

	// 加算演算子
	Vector3 operator+(const Vector3& other) const {
		return { x + other.x, y + other.y, z + other.z };
	}

	// 減算演算子
	Vector3 operator-(const Vector3& other) const {
		return { x - other.x, y - other.y, z - other.z };
	}

	// スカラー乗算演算子
	Vector3 operator*(float scalar) const {
		return { x * scalar, y * scalar, z * scalar };
	}

	// スカラー除算演算子
	Vector3 operator/(float scalar) const {
		return { x / scalar, y / scalar, z / scalar };
	}

	// 等価演算子
	bool operator==(const Vector3& other) const {
		return x == other.x && y == other.y && z == other.z;
	}

	// 非等価演算子
	bool operator!=(const Vector3& other) const {
		return !(*this == other);
	}
};

// 内積
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
	if(mag != 0.0f) {
		return { v.x / mag, v.y / mag, v.z / mag };
	}
	// ゼロベクトルの場合はそのまま返す
	return v;
}

inline Vector3 AddVec3(const Vector3& v1, const Vector3& v2) {
	return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

inline Vector3 MultiplyVec3(float scalar, const Vector3& v) {
	return { scalar * v.x, scalar * v.y, scalar * v.z };
}
