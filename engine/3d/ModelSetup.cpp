/*********************************************************************
 * \file   ModelSetup.cpp
 * \brief
 * 
 * \author Harukichimaru
 * \date   November 2024
 * \note   
 *********************************************************************/
#include "ModelSetup.h"

///=============================================================================
///						初期化
void ModelSetup::Initialize(DirectXCore* dxCore) {
	//========================================
	// 引数で受け取ったDXCoreをセット
	dxCore_ = dxCore;
}