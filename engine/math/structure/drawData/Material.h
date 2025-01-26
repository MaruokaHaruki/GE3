#pragma once
#include <cstdint>
#include "Vector4.h"
#include "Matrix4x4.h"

/// <summary>
/// マテリアル
/// </summary>
struct Material {
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
	float shininess;
};