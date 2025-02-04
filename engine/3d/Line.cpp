/*********************************************************************
 * \file   Line.cpp
 * \brief 
 * 
 * \author Harukichimaru
 * \date   January 2025
 * \note   
 *********************************************************************/
#include "Line.h"
#include "LineSetup.h"
#include "DirectXCore.h"
#include "Camera.h"
#include "TransformationMatrix.h"
#include "Object3dSetup.h"
#include "LineManager.h"
//========================================
// 数学関数のインクルード
#define _USE_MATH_DEFINES
#include <math.h>
#include "AffineTransformations.h"
#include "MathFunc4x4.h"

///=============================================================================
///						初期化
void Line::Initialize(LineSetup* lineSetup) {
	//========================================
	// ラインセットアップの取得
    lineSetup_ = lineSetup;
	//========================================
	// 頂点バッファの作成
    CreateVertexBuffer();
	// トランスフォーメーションマトリックスバッファの作成
	CreateTransformationMatrixBuffer();
	//========================================
	// ワールド行列の初期化
	transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	//========================================
	// カメラの取得
	camera_ = lineSetup_->GetDefaultCamera();
}

///=============================================================================
///						更新
void Line::Update() {
    // カメラの取得
    camera_ = lineSetup_->GetDefaultCamera();

    // ワールド行列の作成
    Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
    Matrix4x4 worldViewProjectionMatrix;

    if(camera_) {
        // ビュー行列とプロジェクション行列を取得
        const Matrix4x4& viewMatrix = camera_->GetViewMatrix();
        const Matrix4x4& projectionMatrix = camera_->GetProjectionMatrix();

        // 行列の乗算（ワールド → ビュー → プロジェクション）
        Matrix4x4 worldViewMatrix = Multiply4x4(worldMatrix, viewMatrix);
        worldViewProjectionMatrix = Multiply4x4(worldViewMatrix, projectionMatrix);
    } else {
        worldViewProjectionMatrix = worldMatrix;
    }

    // 定数バッファへの書き込み
    transformationMatrixData_->WVP = worldViewProjectionMatrix;
    transformationMatrixData_->World = worldMatrix;
    transformationMatrixData_->WorldInvTranspose = Inverse4x4(worldMatrix);
}


///=============================================================================
///						ライン描画
void Line::DrawLine(const Vector3& start, const Vector3& end, const Vector4& color) {
	// 頂点データを追加
	vertices_.push_back({ start, color });
	// 頂点データを追加
    vertices_.push_back({ end, color });
}

///=============================================================================
///                     描画
void Line::Draw() {
	//========================================
	// 描画するラインがない場合は何もしない
	if (vertices_.empty()) return;
	//========================================
	// 描画設定
	void* pData;
	// バーテックスバッファのマップ
	vertexBuffer_->Map(0, nullptr, &pData);
	// メモリコピー
	memcpy(pData, vertices_.data(), sizeof(LineVertex) * vertices_.size());
	// バーテックスバッファのアンマップ
	vertexBuffer_->Unmap(0, nullptr);
	//========================================
	// 描画設定
	auto commandList = lineSetup_->GetDXManager()->GetCommandList();
	// taransformMatrixBufferのマップ
	commandList->SetGraphicsRootConstantBufferView(0, transfomationMatrixBuffer_->GetGPUVirtualAddress()); // 修正: RootParameterIndexを0に変更
	// vertexBufferの設定
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//========================================
	// 描画
	commandList->DrawInstanced(vertices_.size(), 1, 0, 0);
	// NOTE:描画した後はラインをクリアするのを忘れるな
}

///=============================================================================
///						ラインのクリア
void Line::ClearLines() {
	// ラインのクリア
	vertices_.clear();
}

///=============================================================================
///						バーテックスバッファの作成
void Line::CreateVertexBuffer() {
	//========================================
    // デバイスの取得
    auto device = lineSetup_->GetDXManager()->GetDevice();
	// バッファサイズ
	// NOTE: 1000本のラインを描画できるようにしている
    auto bufferSize = sizeof(LineVertex) * 100000000;
    //========================================
	// バーテックスバッファの作成
    D3D12_HEAP_PROPERTIES heapProps = {};
	// ヒープタイプ
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
    //========================================
	// リソースの設定
    D3D12_RESOURCE_DESC bufferDesc = {};
    bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    bufferDesc.Width = bufferSize;
    bufferDesc.Height = 1;
    bufferDesc.DepthOrArraySize = 1;
    bufferDesc.MipLevels = 1;
    bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
    bufferDesc.SampleDesc.Count = 1;
    bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//========================================
	// リソースの作成
    device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&vertexBuffer_)
    );
	//========================================
	// バーテックスバッファビューの設定
    vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	// バイトサイズ
    vertexBufferView_.SizeInBytes = bufferSize;
	// ストライド
    vertexBufferView_.StrideInBytes = sizeof(LineVertex);
}

///=============================================================================
///						
void Line::CreateTransformationMatrixBuffer() {
	// 定数バッファのサイズを 256 バイトの倍数に設定
	size_t bufferSize = (sizeof(TransformationMatrix) + 255) & ~255;
	transfomationMatrixBuffer_ = lineSetup_->GetDXManager()->CreateBufferResource(bufferSize);
	// 書き込み用変数
	TransformationMatrix transformationMatrix = {};
	// 書き込むためのアドレスを取得
	transfomationMatrixBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));
	// 書き込み
	transformationMatrix.WVP = Identity4x4();
	// 単位行列を書き込む
	*transformationMatrixData_ = transformationMatrix;
}

