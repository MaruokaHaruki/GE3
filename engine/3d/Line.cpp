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

///=============================================================================
///						初期化
void Line::Initialize(LineSetup* lineSetup) {
	// ラインセットアップの取得
    lineSetup_ = lineSetup;
	// 頂点バッファの作成
    CreateVertexBuffer();
}

///=============================================================================
///						更新
void Line::Update() {
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
///						グリッド描画
void Line::DrawGrid(const Vector3 &start, const Vector3 &end, const Vector4 &color, int gridNum) {
	//========================================
	// グリッドの描画
	for(int i = 0; i <= gridNum; i++) {
		//========================================
		// 横線
		DrawLine(
			Vector3(start.x, start.y, start.z + i),
			Vector3(end.x, end.y, start.z + i),
			color
		);
		// 縦線
		DrawLine(
			Vector3(start.x + i, start.y, start.z),
			Vector3(start.x + i, end.y, end.z),
			color
		);
	}
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
	// ルートシグネチャのセット
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//========================================
	// 描画
    commandList->DrawInstanced(vertices_.size(), 1, 0, 0);
   //========================================
	// 描画後の処理
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
    auto bufferSize = sizeof(LineVertex) * 1000;
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

