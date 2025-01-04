/*********************************************************************
 * \file   ImguiSetup.h
 * \brief
 *
 * \author Harukichimaru
 * \date   December 2024
 * \note
 *********************************************************************/
#pragma once
//---------------------------------------
// base
#include "WinApp.h"
#include "DirectXCore.h"

//---------------------------------------
// imguiStyle
enum Style {
	DARK,
	LIGHT,
	CLASSIC,
	CYBER,
	GREEN,
};

///=============================================================================
///						ImGuiのセットアップ
class ImguiSetup {

	///--------------------------------------------------------------
	///							メンバ関数
public:

	/// \brief 初期化 使用カラーの設定
	void Initialize(WinApp* winApp, DirectXCore* dxCore, Style style = Style::DARK);

	/**----------------------------------------------------------------------------
	 * \brief  Begin ImGui受付開始処理
	 */
	void Begin();

	/**----------------------------------------------------------------------------
	 * \brief  End ImGui受付終了処理
	 */
	void End();

	/**----------------------------------------------------------------------------
	 * \brief  Draw 描画
	 */
	void Draw();

	/**----------------------------------------------------------------------------
	 * \brief  Finalize 終了処理
	 */
	void Finalize();

	/**----------------------------------------------------------------------------
	 * \brief  StyleColorsCyberGreen ImGuiのサイバースタイル
	 * \param  style スタイル
	 */
	void StyleColorsCyberGreen(ImGuiStyle& style);

	/**----------------------------------------------------------------------------
	 * \brief  StyleColorsDarkGreen
	 * \param  style
	 */
	void StyleColorsDarkGreen(ImGuiStyle& style);

	/**----------------------------------------------------------------------------
	 * \brief  ShowPerformanceMonitor
	 */
	void ShowPerformanceMonitor();

	///--------------------------------------------------------------
	///							静的メンバ関数
private:

	///--------------------------------------------------------------
	///							入出力関数
public:


	///--------------------------------------------------------------
	///							メンバ変数
private:

	//========================================
	// WinAppのポインタ
	WinApp* winApp_ = nullptr;
	// DirectXCoreのポインタ
	DirectXCore* dxCore_ = nullptr;

	//========================================
	// SRV用ディスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_ = nullptr;

	// 描画コール数などのカウンタ（例: 自身のエンジンやゲームループで管理）
	int drawCallCount = 0;
	int triangleCount = 0;
	float logicTime = 0.0f;      // ゲームロジック処理時間（ms）
	float renderingTime = 0.0f;  // レンダリング処理時間（ms）
};

