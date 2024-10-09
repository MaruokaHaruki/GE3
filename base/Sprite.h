///===================================================================///
///							Spriteクラス.h
///===================================================================///
//NOTE:スプライト1枚分のクラス
#include <cstdint>
//DX12include
#include<d3d12.h>
#include<dxgi1_6.h>
#include <wrl/client.h>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
//DXC
#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")
//自作クラス
#include "VertexData.h"
#include "Material.h"
#include "TransformationMatrix.h"
#include "Transform.h"
#include "Matrix4x4.h"
#include <Calc4x4.h>

#pragma once
class SpriteManager;
class Sprite {
	///----------------------------------------------------///
	///						メンバ関数
	///----------------------------------------------------///
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(SpriteManager* spriteManager);

	/// <summary>
	/// 更新
	/// </summary>
	void Update(Transform transform,Matrix4x4 viewMatrix = Identity4x4());

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(D3D12_GPU_DESCRIPTOR_HANDLE textureHandle);

private:
	/// <summary>
	///	頂点バッファの作成
	/// </summary>
	void CreateVertexBuffer();

	/// <summary>
	/// インデックスバッファの作成
	/// </summary>
	void CreateIndexBuffer();

	/// <summary>
	/// マテリアルバッファの作成
	/// </summary>
	void CreateMaterialBuffer();

	/// <summary>
	/// トランスフォーメーションマトリックスバッファの作成
	/// </summary>
	void CreateTransformationMatrixBuffer();

	///----------------------------------------------------///
	///						メンバ変数
	///----------------------------------------------------///
private:
	///====================スプライト共通部Pointer====================///
	//スプライトマネージャ
	SpriteManager* spriteManager_ = nullptr;

	///====================バッファデータ====================///
	//頂点
	Microsoft::WRL::ComPtr <ID3D12Resource> vertexBuffer_;
	//インデックス
	Microsoft::WRL::ComPtr <ID3D12Resource> indexBuffer_;
	//マテリアル
	Microsoft::WRL::ComPtr <ID3D12Resource> materialBuffer_;
	//トランスフォーメーションマトリックス
	Microsoft::WRL::ComPtr <ID3D12Resource> transfomationMatrixBuffer_;

	///====================バッファリソース内のデータを指すポインタ====================///
	//頂点
	VertexData* vertexData_ = nullptr;
	//インデックス
	uint32_t* indexData_ = nullptr;
	//マテリアル
	Material* materialData_ = nullptr;
	//トランスフォーメーションマトリックス
	TransformationMatrix* transformationMatrixData_ = nullptr;

	///====================バッファリソースの使い道を指すポインタ====================///
	//頂点
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	//インデックス
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;


	///====================その他データ====================///
};

