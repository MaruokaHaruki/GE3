/*********************************************************************
 * \file   ModelSetup.h
 * \brief  モデル共通部クラス
 *
 * \author Harukichimaru
 * \date   November 2024
 * \note
 *********************************************************************/
#pragma once
#include "DirectXCore.h"

 ///=============================================================================
 ///						モデル共通部クラス
class ModelSetup {

	///--------------------------------------------------------------
	///							メンバ関数
public:

	/// \brief 初期化
	void Initialize(DirectXCore* dxCore);

	///--------------------------------------------------------------
	///							静的メンバ関数
private:

	///--------------------------------------------------------------
	///							入出力関数
public:

	/**----------------------------------------------------------------------------
	 * \brief  GetDXManager DirectXCore取得
	 * \return
	 * \note
	 */
	DirectXCore* GetDXManager() const { return dxCore_; }


	///--------------------------------------------------------------
	///							メンバ変数
private:
	//---------------------------------------
	// DirectXCoreポインタ
	DirectXCore* dxCore_ = nullptr;
};

