/*********************************************************************
 * \file   ParticleManager.h
 * \brief
 *
 * \author Harukichimaru
 * \date   December 2024
 * \note
 *********************************************************************/
#pragma once
#include <random>
#include "DirectXCore.h"
#include "SrvSetup.h"
#include "Camera.h"
//---------------------------------------
// データ構造体
#include "VertexData.h"
#include "ModelData.h"
#include "Transform.h"
#include "Matrix4x4.h"
#include "Material.h"

//Particle構造体
struct Particle {
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

// パーティクルグループ構造体
struct ParticleGroup {
	// マテリアルデータ (テクスチャファイルパスとテクスチャ用SRVインデックス)
	std::string materialFilePath;
	// パーティクルのリスト (std::list<Particle>型)
	std::list<Particle> particleList;
	// インスタンシングデータ用SRVインデックス
	int instancingSrvIndex = -1; // 初期化
	// インスタンシングリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource = nullptr;
	// インスタンス数
	UINT instanceCount = 0;
	// インスタンシングデータを書き込むためのポインタ
	ParticleForGPU* instancingDataPtr = nullptr;
};

///=============================================================================
///						クラス
class ParticleManager {
	///--------------------------------------------------------------
	///							メンバ関数
public:
	/**----------------------------------------------------------------------------
	* \brief Getinstance
	* \return インスタンス
	*/
	static ParticleManager* GetInstance();

	/// \brief 初期化
	void Initialize(DirectXCore* dxCore, const std::string& textureDirectoryPath, SrvSetup* srvSetup);

	void Finalize();

	/// \brief 更新
	void Update(Camera camera);

	/// \brief 描画 
	void Draw();

	/**----------------------------------------------------------------------------
	 * \brief  Emit nameで指定したパーティクルグループにパーティクルを発生させる
	 * \param  groupName
	 * \param  position
	 * \param  count
	 */
	void Emit(const std::string& groupName, const Vector3& position, uint32_t count);

	/**----------------------------------------------------------------------------
	 * \brief  CreatePathcleGroup 
	 * \param  groupName
	 * \param  textureFilePath
	 */
	void CreatePathcleGroup(const std::string& groupName, const std::string& textureFilePath);

	///--------------------------------------------------------------
	///							静的メンバ関数
private:


	/**----------------------------------------------------------------------------
	* \brief  CreateRootSignature ルートシグネチャーの作成
	*/
	void CreateRootSignature();

	/**----------------------------------------------------------------------------
	* \brief  CreateGraphicsPipeline グラフィックスパイプラインの作成
	*/
	void CreateGraphicsPipeline();

	/**----------------------------------------------------------------------------
	* \brief  CreateVertexData 頂点リソースを作成
	*/
	void CreateVertexData();

	/**----------------------------------------------------------------------------
  * \brief  VertexBufferView 頂点バッファビューの作成
  */
	void CreateVertexBufferView();

	/**----------------------------------------------------------------------------
  * \brief  CreateMaterialBuffer マテリアルバッファの作成
  */
	void CreateMaterialBuffer();

	///--------------------------------------------------------------
	///							入出力関数
public:


	///--------------------------------------------------------------
	///							メンバ変数
private:
	//========================================
	// シングルトンインスタンス
	static ParticleManager* instance_;

	//========================================
private:
	ParticleManager() = default;
	~ParticleManager() = default;
	ParticleManager(ParticleManager&) = delete;
	ParticleManager& operator=(ParticleManager&) = delete;



	///Setup
	//========================================
	// ダイレクトXマネージャー
	DirectXCore* dxCore_ = nullptr;
	// SRVセットアップ
	SrvSetup* srvSetup_ = nullptr;
	//ランダムエンジン
	std::mt19937 randomEngine_;

	//========================================
	// RootSignature
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	// グラフィックスパイプライン
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;






	///メイン
	//---------------------------------------
	// パーティクルグループのコンテナ
	std::unordered_map<std::string, ParticleGroup> particleGroups_;

	//---------------------------------------
	// モデルデータ
	ModelData modelData_;

	//---------------------------------------
	// 頂点データ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;
	//マテリアル
	Microsoft::WRL::ComPtr <ID3D12Resource> materialBuffer_;

	///---------------------------------------
	/// バッファリソース内のデータを指すポインタ
	//頂点
	VertexData* vertexData_ = nullptr;
	//マテリアル
	Material* materialData_ = nullptr;

	///---------------------------------------
	/// バッファリソースの使い道を指すポインタ
	//頂点
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};


	//========================================
	// パーティクルの最大出力数
	const uint32_t kNumMaxInstance_ = 128;
	//とりあえず60fps固定してあるが、実時間を計測して可変fpsで動かせるようにしておくとなおよい
	const float kDeltaTime_ = 1.0f / 60.0f;
};
