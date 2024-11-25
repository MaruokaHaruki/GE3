/*********************************************************************
 * \file   Object3d.h
 * \brief  
 * 
 * \author Harukichimaru
 * \date   November 2024
 * \note   
 *********************************************************************/
#pragma once
#include "MaterialData.h"
#include "ModelData.h"
#include "VertexData.h"
#include "Material.h"
#include "TransformationMatrix.h"
#include "DirectionalLight.h"
#include "Transform.h"
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

class Object3dSetup;
class Object3d {
	///--------------------------------------------------------------
	///							メンバ関数
	public:

	/// \brief 初期化
	void Initialize(Object3dSetup* object3dSetup);

	/// \brief 更新
	void Update();

	/// \brief 描画 
	void Draw();

	///--------------------------------------------------------------
	///						 静的メンバ関数
private:
	/**----------------------------------------------------------------------------
	 * \brief  LoadMaterialTemplateFile .mtlファイル読み込み
	 * \param  directoryPath ディレクトリパス
	 * \param  filename ファイルネーム
	 * \return materialData マテリアルデータ
	 * \note   
	 */
	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	/**----------------------------------------------------------------------------
	 * \brief  LoadObjFile .objファイル読み込み
	 * \param  directoryPath ディレクトリパス
	 * \param  filename ファイルネーム
	 * \note   そのままmodelDataに格納
	 */
	void LoadObjFile(const std::string& directoryPath, const std::string& filename);

	/**----------------------------------------------------------------------------
	 * \brief  頂点バッファの作成
	 * \note
	 */
	void CreateVertexBuffer();

	/**----------------------------------------------------------------------------
	 * \brief  インデックスバッファの作成
	 * \note
	 */
	//void CreateIndexBuffer();

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
	 * \brief  並行光源の作成
	 * \note
	 */
	void CreateDirectionalLight();

	///--------------------------------------------------------------
	///							入出力関数
public:


	///--------------------------------------------------------------
	///							メンバ変数
private:

	//---------------------------------------
	// オブジェクト3Dセットアップポインタ
	Object3dSetup* object3dSetup_ = nullptr;

	//---------------------------------------
	// モデルデータ
	ModelData modelData_;

	//---------------------------------------
	// 頂点データ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;
	//インデックス
	//Microsoft::WRL::ComPtr <ID3D12Resource> indexBuffer_;
	//マテリアル
	Microsoft::WRL::ComPtr <ID3D12Resource> materialBuffer_;
	//トランスフォーメーションマトリックス
	Microsoft::WRL::ComPtr <ID3D12Resource> transfomationMatrixBuffer_;
	//並行光源
	Microsoft::WRL::ComPtr <ID3D12Resource> directionalLightBuffer_;

	///---------------------------------------
	/// バッファリソース内のデータを指すポインタ
	//頂点
	VertexData* vertexData_ = nullptr;
	//インデックス
	//uint32_t* indexData_ = nullptr;
	//マテリアル
	Material* materialData_ = nullptr;
	//トランスフォーメーションマトリックス
	TransformationMatrix* transformationMatrixData_ = nullptr;
	//並行光源
	DirectionalLight* directionalLightData_ = nullptr;

	///---------------------------------------
	/// バッファリソースの使い道を指すポインタ
	//頂点
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	//インデックス
	//D3D12_INDEX_BUFFER_VIEW indexBufferView_;


	//---------------------------------------
	// テスト用変数
	uint32_t textureIndex_ = 0;

	///--------------------------------------
	/// Transform
	Transform transform_;
	Transform cameraTransform_;
	
};

