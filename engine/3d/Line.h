/*********************************************************************
 * \file   Line.h
 * \brief
 * 
 * \author Harukichimaru
 * \date   January 2025
 * \note   
 *********************************************************************/
#pragma once
#include <vector>
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

struct LineVertex {
	Vector3 position;
	Vector4 color;
};

class Camera;
class LineSetup;
class Line {
	///--------------------------------------------------------------
	///							メンバ関数
public:

	/// \brief 初期化
	void Initialize(LineSetup *lineSetup);

	/// \brief 更新
	void Update();

	/// \brief 描画 
	void Draw();

	void DrawLine(const Vector3 &start, const Vector3 &end, const Vector4 &color);

	///--------------------------------------------------------------
	///						 静的メンバ関数
private:
	/**----------------------------------------------------------------------------
	 * \brief  CreateVertexBuffer 頂点バッファの作成
	 */
	void CreateVertexBuffer();

	///--------------------------------------------------------------
	///							入出力関数
public:

	///--------------------------------------------------------------
	///							メンバ変数
private:

	//---------------------------------------
	// オブジェクト3Dセットアップポインタ
	LineSetup* lineSetup_ = nullptr;

	//---------------------------------------
	// 頂点データ
	std::vector<LineVertex> vertices_;

	//---------------------------------------
	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;
	// バッファリソースの使い道を指すポインタ
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	////---------------------------------------
	////トランスフォーメーションマトリックス
	//Microsoft::WRL::ComPtr <ID3D12Resource> transfomationMatrixBuffer_;

	////---------------------------------------
	//// バッファリソース内のデータを指すポインタ
	////トランスフォーメーションマトリックス
	//TransformationMatrix* transformationMatrixData_ = nullptr;
	////並行光源
	//DirectionalLight* directionalLightData_ = nullptr;

	////--------------------------------------
	//// Transform
	//Transform transform_ = {};

	//--------------------------------------
	// カメラ
	Camera* camera_ = nullptr;
};

