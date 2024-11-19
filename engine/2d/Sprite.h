/*********************************************************************
 * \file   Sprite.h
 * \brief  スプライト
 *
 * \author Harukichimaru
 * \date   October 2024
 * \note   スプライト1枚分のクラス
 *********************************************************************/
#pragma once
 //========================================
 // Windows include
#include <cstdint>
//========================================
// DX12include
#include<d3d12.h>
#include<dxgi1_6.h>
#include <wrl/client.h>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
//========================================
// DXC
#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")
//========================================
// 自作構造体
#include "VertexData.h"
#include "Material.h"
#include "TransformationMatrix.h"
#include "Transform.h"
#include "Matrix4x4.h"
//========================================
// 自作クラス
#include "Calc4x4.h"
#include "TextureManager.h"

///=============================================================================
///								クラス
class SpriteSetup;
class Sprite {
	///--------------------------------------------------------------
	///						 メンバ関数
public:

	/**----------------------------------------------------------------------------
	 * \brief  初期化
	 * \param  spriteManager スプライト管理クラス
	 * \param  textureFilePath ファイルパス
	 * \note
	 */
	void Initialize(SpriteSetup* spriteManager, std::string textureFilePath);

	/**----------------------------------------------------------------------------
	 * \brief  更新
	 * \param  viewMatrix ビュー行列
	 * \note   ビュー行列はいれるとなんか便利な事ができる
	 */
	void Update(Matrix4x4 viewMatrix = Identity4x4());

	/**----------------------------------------------------------------------------
	 * \brief  描画
	 * \param  textureHandle テクスチャハンドルの取得
	 * \note
	 */
	void Draw();

	///--------------------------------------------------------------
	///						 ローカルメンバ関数
private:
	/**----------------------------------------------------------------------------
	 * \brief  頂点バッファの作成
	 * \note
	 */
	void CreateVertexBuffer();

	/**----------------------------------------------------------------------------
	 * \brief  インデックスバッファの作成
	 * \note
	 */
	void CreateIndexBuffer();

	/**----------------------------------------------------------------------------
	 * \brief  マテリアルバッファの作成
	 * \note
	 */
	void CreateMaterialBuffer();

	/**----------------------------------------------------------------------------
	 * \brief  トランスフォーメーションマトリックスバッファの作成
	 * \note
	 */
	void CreateTransformationMatrixBuffer();

	/**----------------------------------------------------------------------------
	 * \brief  ReflectSRT SRTの反映
	 * \note
	 */
	void ReflectSRT();

	/**----------------------------------------------------------------------------
	 * \brief  ReflectAnchorPoint アンカーポイントとフリップの反映
	 * \note
	 */
	void ReflectAnchorPointAndFlip();

	/**----------------------------------------------------------------------------
	 * \brief  ReflectTextureRange テクスチャ範囲指定の反映
	 * \note
	 */
	void ReflectTextureRange();

	/**----------------------------------------------------------------------------
	 * \brief  AdjustTextureSize テクスチャサイズの調整
	 * \note
	 */
	void AdjustTextureSize();



	///--------------------------------------------------------------
	///						 入出力関数
public:
	/**----------------------------------------------------------------------------
	 * \brief  GetPosition 座標の取得
	 * \return position 座標
	 * \note
	 */
	const Vector2& GetPosition() const { return position_; }
	/**----------------------------------------------------------------------------
	 * \brief  position 座標の設定
	 * \param  position
	 * \note
	 */
	void SetPosition(const Vector2& position) { this->position_ = position; }


	/**----------------------------------------------------------------------------
	 * \brief  GetRotation 回転の取得
	 * \return rotation_
	 * \note
	 */
	const float& GetRotation() const { return rotation_; }
	/**----------------------------------------------------------------------------
	 * \brief  SetRotation 回転の設定
	 * \param  rotation
	 * \note
	 */
	void SetRotation(float rotation) { this->rotation_ = rotation; }


	/**----------------------------------------------------------------------------
	 * \brief  GetColor 色の取得
	 * \return 色
	 * \note
	 */
	const Vector4& GetColor() const { return materialData_->color; }
	/**----------------------------------------------------------------------------
	 * \brief  SetColor 色の設定
	 * \param  color
	 * \note
	 */
	void SetColor(const Vector4& color) { materialData_->color = color; }


	/**----------------------------------------------------------------------------
	 * \brief  GetSize 大きさの取得
	 * \return
	 * \note
	 */
	const Vector2 GetSize() const { return size_; }
	/**----------------------------------------------------------------------------
	 * \brief  SetSize 大きさの設定
	 * \param  size
	 * \note
	 */
	void SetSize(const Vector2& size) { this->size_ = size; }


	/**----------------------------------------------------------------------------
	 * \brief  SetTexture テクスチャの差し替え
	 * \param  textureFilePath
	 * \note
	 */
	void SetTexture(std::string& textureFilePath) { textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureFilePath); }


	/**----------------------------------------------------------------------------
	 * \brief  GetAnchorPoint アンカーポイントの取得
	 * \return anchorPoint_
	 * \note
	 */
	const Vector2& GetAnchorPoint() const { return anchorPoint_; }
	/**----------------------------------------------------------------------------
	 * \brief  SetAnchorPoint アンカーポイントの設定
	 * \param  anchorPoint アンカーポイント
	 * \note
	 */
	void SetAnchorPoint(const Vector2& anchorPoint) { this->anchorPoint_ = anchorPoint; }


	/**----------------------------------------------------------------------------
		 * \brief  GetFlipX 左右フリップの取得
		 * 	 * \return isFlipX_
		 * 	 * \note
		 * 	 */
	const bool& GetFlipX() const { return isFlipX_; }
	/**----------------------------------------------------------------------------
	* \brief  SetFlipX 左右フリップの設定
	* \param  isFlipX
	* \note
	*/
	void SetFlipX(bool isFlipX) { this->isFlipX_ = isFlipX; }


	/**----------------------------------------------------------------------------
	 * \brief  GetFlipY 上下フリップの取得
	* \return isFlipY_
	* \note
	*/
	const bool& GetFlipY() const { return isFlipY_; }
	/**----------------------------------------------------------------------------
	* \brief  SetFlipY 上下フリップの設定
	* \param  isFlipY
	* \note
	*/
	void SetFlipY(bool isFlipY) { this->isFlipY_ = isFlipY; }


	/**----------------------------------------------------------------------------
	 * \brief  GetTextureLeftTop テクスチャ左上座標の取得
	 * \return textureLeftTop_ テクスチャ左上座標
	 * \note
	 */
	const Vector2& GetTextureLeftTop() const { return textureLeftTop_; }
	/**----------------------------------------------------------------------------
	 * \brief  SetTextureLeftTop テクスチャ左上座標の設定
	 * \param  textureLeftTop テクスチャ左上座標
	 * \note
	 */
	void SetTextureLeftTop(const Vector2& textureLeftTop) { this->textureLeftTop_ = textureLeftTop; }


	/**----------------------------------------------------------------------------
	 * \brief  GetTextureSize テクスチャ切り出しサイズの取得
	 * \return textureSize_ テクスチャ切り出しサイズ
	 * \note
	 */
	const Vector2& GetTextureSize() const { return textureSize_; }
	/**----------------------------------------------------------------------------
	 * \brief  SetTextureSize テクスチャ切り出しサイズの設定
	 * \param  textureSize テクスチャ切り出しサイズ
	 * \note
	 */
	void SetTextureSize(const Vector2& textureSize) { this->textureSize_ = textureSize; }


	///--------------------------------------------------------------
	///						 メンバ変数
private:
	///---------------------------------------
	/// スプライトマネージャ
	SpriteSetup* spriteSetup_ = nullptr;

	///---------------------------------------
	/// バッファデータ
	//頂点
	Microsoft::WRL::ComPtr <ID3D12Resource> vertexBuffer_;
	//インデックス
	Microsoft::WRL::ComPtr <ID3D12Resource> indexBuffer_;
	//マテリアル
	Microsoft::WRL::ComPtr <ID3D12Resource> materialBuffer_;
	//トランスフォーメーションマトリックス
	Microsoft::WRL::ComPtr <ID3D12Resource> transfomationMatrixBuffer_;

	///---------------------------------------
	/// バッファリソース内のデータを指すポインタ
	//頂点
	VertexData* vertexData_ = nullptr;
	//インデックス
	uint32_t* indexData_ = nullptr;
	//マテリアル
	Material* materialData_ = nullptr;
	//トランスフォーメーションマトリックス
	TransformationMatrix* transformationMatrixData_ = nullptr;

	///---------------------------------------
	/// バッファリソースの使い道を指すポインタ
	//頂点
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	//インデックス
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;

	///---------------------------------------
	/// SRT設定
	//トランスフォーム
	Transform transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	// 座標
	Vector2 position_ = { 0.0f,0.0f };
	// 回転
	float rotation_ = 0.0f;
	// サイズ
	Vector2 size_ = { 1.0f,1.0f };

	///---------------------------------------
	/// テクスチャ番号
	uint32_t textureIndex = 0;

	///---------------------------------------
	/// アンカーポイント
	Vector2 anchorPoint_ = { 0.0f,0.0f };

	///---------------------------------------
	/// フリップ
	//左右フリップ
	bool isFlipX_ = false;
	//上下フリップ
	bool isFlipY_ = false;

	///---------------------------------------
	/// テクスチャ範囲指定
	//テクスチャ左上座標
	Vector2 textureLeftTop_ = { 0.0f,0.0f };
	//テクスチャ切り出しサイズ
	Vector2 textureSize_ = { 0.0f,0.0f };
};

