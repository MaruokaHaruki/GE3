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
class SpriteManager;
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
	void Initialize(SpriteManager* spriteManager, std::string textureFilePath);

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


	///--------------------------------------------------------------
	///						 メンバ変数
private:
	///---------------------------------------
	/// スプライトマネージャ
	SpriteManager* spriteManager_ = nullptr;

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
};

