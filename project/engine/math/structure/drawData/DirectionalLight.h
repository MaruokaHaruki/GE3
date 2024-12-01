#pragma once
#include "Vector3.h"
#include "Vector4.h"

/// <summary>
/// 並行光源
/// NOTE:光源の色、向き、光度表す。向きは必ず正規化しておくこと
/// </summary>
struct DirectionalLight {
	Vector4 color;		//ライトの色
	Vector3 direction;	//ライトの向き
	float intensity;	//光度
};