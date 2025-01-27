/*********************************************************************
 * \file   SrvSetup.cpp
 * \brief
 *
 * \author Harukichimaru
 * \date   December 2024
 * \note
 *********************************************************************/
#include "SrvSetup.h"

///=============================================================================
///						初期化
void SrvSetup::Initialize(DirectXCore* dxCore) {
	//========================================
	// DXCoreの設定
	this->dxCore_ = dxCore;

	//========================================
	// ディスクリプタヒープの生成
	descriptorHeap_ = dxCore_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxSRVCount_, true);
	//ディスクリプタ1個分のサイズを取得して記録
	descriptorSizeSRV_ = dxCore_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

///=============================================================================
///						ループ前処理
void SrvSetup::PreDraw() {
	ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap_.Get() };
	dxCore_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
}

///=============================================================================
///						メモリ確保
uint32_t SrvSetup::Allocate() {
	// returnする番号を一旦記録
	uint32_t index = useIndex_;
	//次に使用するディスクリプタのインデックスを進める
	useIndex_++;
	//上で記録した番号を返す
	return index;
}

///=============================================================================
///						SRV生成(テクスチャ用)
void SrvSetup::CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT format, UINT mipLevels) {
	//========================================
	// ディスクリプタハンドルの取得
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += ( descriptorSizeSRV_ * srvIndex );
	//========================================
	// テクスチャ用のSRVを生成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = mipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	dxCore_->GetDevice()->CreateShaderResourceView(pResource, &srvDesc, handleCPU);
}

///=============================================================================
///						SRV生成(構造化バッファ用)
void SrvSetup::CreateSRVStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT enelemtQuantity, UINT structureByteStride) {
	//========================================
	// ディスクリプタハンドルの取得
	//D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	//handleCPU.ptr += ( descriptorSizeSRV_ * srvIndex );
	//========================================
	// 構造化バッファ用のSRVを生成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.StructureByteStride = structureByteStride;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = enelemtQuantity;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	dxCore_->GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetSRVCPUDescriptorHandle(srvIndex));
}

///=============================================================================
///						SRVDescriptorHandleの取得を関数化
///--------------------------------------------------------------
///						 CPU
D3D12_CPU_DESCRIPTOR_HANDLE SrvSetup::GetSRVCPUDescriptorHandle(uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += ( descriptorSizeSRV_ * index );
	return handleCPU;
}

///--------------------------------------------------------------
///						 GPU
D3D12_GPU_DESCRIPTOR_HANDLE SrvSetup::GetSRVGPUDescriptorHandle(uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += ( descriptorSizeSRV_ * index );
	return handleGPU;

}

void SrvSetup::SetGraphicsRootDescriptorTable(uint32_t rootParameterIndex, uint32_t srvIndex) {
	//========================================
	// ルートディスクリプタテーブルの設定
	dxCore_->GetCommandList()->SetGraphicsRootDescriptorTable(rootParameterIndex, GetSRVGPUDescriptorHandle(srvIndex));
}
