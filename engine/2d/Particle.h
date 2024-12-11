/*********************************************************************
 * \file   Particl.h
 * \brief
 *
 * \author Harukichimaru
 * \date   December 2024
 * \note
 *********************************************************************/
#pragma once
#include "ParticleSetup.h"
#include "ModelData.h"
#include "VertexData.h"
#include "Material.h"

//========================================
// 標準ライブラリ
#include <random>

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

//Particle構造体
struct ParticleStr {
	Transform transform;
	Vector3 velocity;
	Vector4 color;
	float lifeTime;
	float currentTime;
};

struct ParticleForGPU {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Vector4 color;
};

// パーティクルグループ構造体の定義
struct ParticleGroup {
	// マテリアルデータ
	std::string materialFilePath;
	int srvIndex;
	// パーティクルのリスト (std::list<ParticleStr>型)
	std::list<ParticleStr> particleList;
	// インスタンシングデータ用SRVインデックス
	int instancingSrvIndex;
	// インスタンシングリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource = nullptr;
	// インスタンス数
	UINT instanceCount = 0;
	// インスタンシングデータを書き込むためのポインタ
	ParticleForGPU* instancingDataPtr = nullptr;

	Vector2 textureLeftTop = { 0.0f, 0.0f }; // テクスチャ左上座標
	Vector2 textureSize = { 0.0f, 0.0f }; // テクスチャサイズを追加
};

class Object3dSetup;
class Camera;
class Particle {
	///--------------------------------------------------------------
	///							メンバ関数
public:

	/// \brief 初期化
	void Initialize(ParticleSetup* particleSetup);

	/// \brief 更新
	void Update();

	/// \brief 描画 
	void Draw();

	/**----------------------------------------------------------------------------
	 * \brief  Emit
	 * \param  name
	 * \param  position
	 * \param  count
	 */
	void Emit(const std::string name, const Vector3& position, uint32_t count);

	/**----------------------------------------------------------------------------
	 * \brief  CreateParticleGroup
	 * \param  name
	 * \param  materialFilePath
	 * \param  maxInstanceCount
	 */
	void CreateParticleGroup(const std::string& name, const std::string& textureFilePath, uint32_t maxInstanceCount);


	///--------------------------------------------------------------
	///						 静的メンバ関数
private:
	/**----------------------------------------------------------------------------
	 * \brief  CreateVertexData 頂点データの作成
	 */
	void CreateVertexData();

	/**----------------------------------------------------------------------------
	 * \brief  CreateVertexBufferView 頂点バッファビューの作成
	 */
	void CreateVertexBufferView();

	/**----------------------------------------------------------------------------
	 * \brief  CreateMaterialData マテリアルデータの作成
	 */
	void CreateMaterialData();

	/**----------------------------------------------------------------------------
	 * \brief  CreateNewParticle 新しいパーティクルを生成
	 * \param  randomEngine 乱数生成器
	 * \param  position 生成位置
	 */
	ParticleStr CreateNewParticle(std::mt19937& randomEngine, const Vector3& position);

	///--------------------------------------------------------------
	///							入出力関数
public:


	///--------------------------------------------------------------
	///							メンバ変数
private:

	//---------------------------------------
	// パーティクルセットアップポインタ
	ParticleSetup* particleSetup_ = nullptr;

	//---------------------------------------
	// パーティクルグループ
	std::unordered_map<std::string, ParticleGroup> particleGroups;

	//---------------------------------------
	// モデルデータ
	ModelData modelData_;

	//---------------------------------------
	// 頂点データ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;
	// バッファリソースの使い道を指すポインタ
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	// バッファリソース内のデータを指すポインタ
	VertexData* vertexData_ = nullptr;

	//---------------------------------------
	// マテリアルデータ
	Microsoft::WRL::ComPtr<ID3D12Resource> materialBuffer_;
	// バッファリソース内のデータを指すポインタ
	Material* materialData_ = nullptr;

	//---------------------------------------
	// インスタンシングバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingBuffer_;

	//---------------------------------------
	// 乱数生成器の初期化
	std::random_device seedGenerator_;
	std::mt19937 randomEngine_;

	//---------------------------------------
	// その他
	// カメラ目線を使用するかどうか
	bool isUsedBillboard = true;
	//最大インスタンス数
	static const uint32_t kNumMaxInstance = 128;
	//
	const float kDeltaTime = 1.0f / 60.0f;
	// 乱数範囲の調整用
	struct RangeForRandom {
		float min;
		float max;
	};

	RangeForRandom translateRange_ = { 0.0f, 0.0f };
	RangeForRandom colorRange_ = { 1.0f, 1.0f };
	RangeForRandom lifetimeRange_ = { 1.0f, 3.0f };
	RangeForRandom velocityRange_ = { -1.1f, 1.1f };

	Vector2 customTextureSize = { 10.0f, 10.0f };
};

