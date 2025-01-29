/*********************************************************************
 * \file   LineManager.h
 * \brief  
 * 
 * \author Harukichimaru
 * \date   January 2025
 * \note   
 *********************************************************************/
#pragma once
#include <vector>
#include <memory>
#include "Line.h"
#include "LineSetup.h"
#include "DirectXCore.h"
#include "SrvSetup.h"

///=============================================================================
///						ラインマネージャ
class LineManager {
    ///--------------------------------------------------------------
	///						 メンバ関数
public:
	/**----------------------------------------------------------------------------
	 * \brief  GetInstance インスタンスの取得
	 * \return 
	 */
	static LineManager *GetInstance();

    /**----------------------------------------------------------------------------
	 * \brief  Initialize 初期化
     * \param  dxCore
     */
	void Initialize(DirectXCore *dxCore, SrvSetup *srvSetup);

	/**----------------------------------------------------------------------------
	 * \brief  Finalize 終了処理
	 */
	void Finalize();

	/**----------------------------------------------------------------------------
	 * \brief  Update 更新
	 */
	void Update();

	/**----------------------------------------------------------------------------
	 * \brief  Draw 描画
	 */
	void Draw();

	/**----------------------------------------------------------------------------
	 * \brief  DrawImGui ImGuiの描画
	 */
	void DrawImGui();

	/**----------------------------------------------------------------------------
	* \brief  ClearLines ラインのクリア
	*/
	void ClearLines();

	/**----------------------------------------------------------------------------
	 * \brief  AddLine ラインの描画
	 * \param  start 始点
	 * \param  end 終点
	 * \param  color 色
	 */
    void DrawLine(const Vector3& start, const Vector3& end, const Vector4& color);

	/**----------------------------------------------------------------------------
	 * \brief  DrawGrid グリッドの描画
	 * \param  start 始点
	 * \param  end 終点
	 * \param  color 色
	 * \param  gridNum グリッド数
	 */
	void DrawGrid(const Vector3 &start, const Vector3 &end, const Vector4 &color, int gridNum);

	///--------------------------------------------------------------
	///						 静的メンバ関数
public:
	
	/**----------------------------------------------------------------------------
	 * \brief  GetDefaultCamera デフォルトカメラの取得
	 * \return 
	 */
	Camera *GetDefaultCamera() { return lineSetup_->GetDefaultCamera(); }

	void SetDefaultCamera(Camera *camera) { lineSetup_->SetDefaultCamera(camera); }
		
    ///--------------------------------------------------------------
    ///						 メンバ変数
private:
    //========================================
    // コンストラクタを非公開に
	static LineManager* instance_;

	//コンストラクタ
	LineManager() = default;
	//デストラクタ
	~LineManager() = default;
	//コピーコンストラクタ
	LineManager(const LineManager&) = delete;	
	//代入演算子
	LineManager& operator=(const LineManager&) = delete;

    //========================================
    // DirectXCore への参照
    DirectXCore* dxCore_ = nullptr;
	// SrvSetup への参照
	SrvSetup *srvSetup_ = nullptr;

	//========================================
    // ライン
	std::unique_ptr<Line> line_;

	//========================================
    // LineSetup インスタンス
    std::unique_ptr<LineSetup> lineSetup_;
};
