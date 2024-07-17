#include <cstdint>
#include <string>
#include <format>
#include<cassert>
#include <wrl.h>
//自作関数
#include"utils/WstringConve.h"
#include "utils/Log.h"
#include "WinApp.h"
//ReportLiveObj
#include <dxgidebug.h>
#pragma comment(lib,"dxguid.lib")
//DX12include
#include<d3d12.h>
#include<dxgi1_6.h>
#include <wrl/client.h>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")


#pragma once
class DirectXManager {
public:

	///========================================================///
	//コンストラクタ・デストラクタ
	///========================================================///
	/// <summary>
	/// コンストラクタ
	/// </summary>
	DirectXManager();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~DirectXManager();

	///========================================================///
	//ダイレクトXの初期化
	///========================================================///
	
	/// <summary>
	/// 
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 
	/// </summary>
	void PostDraw();

	/// <summary>
	/// ダイレクトXの初期化
	/// </summary>
	void InitializeDirectX(WinApp* winApp);

	/// <summary>
	/// 開放処理
	/// </summary>
	/// <param name="hwnd"></param>
	void ReleaseDirectX();

	/// <summary>
	/// デバックレイヤーの生成
	/// </summary>
	void CreateDebugLayer();

	/// <summary>
	/// DXGIファクトリーの生成
	/// </summary>
	/// <param name="str"></param>
	/// <returns></returns>
	void CreateDxgiFactory();

	/// <summary>
	/// 使用するアダプタ用変数
	/// </summary>
	void SelectAdapter();

	/// <summary>
	/// D3D12Deviceの作成
	/// </summary>
	void CreateD3D12Device();

	/// <summary>
	/// エラー・警告の場合即停止(初期化完了のあとに行う)
	/// </summary>
	void SetupErrorHandling();

	/// <summary>
	/// コマンドキューを作成する
	/// </summary>
	void CreateCommandQueue();

	/// <summary>
	/// コマンドアロケータを生成する
	/// </summary>
	void CreateCommandAllocator();

	/// <summary>
	/// スワップチェーンを生成する
	/// </summary>
	void CreateSwapChain();

	/// <summary>
	/// FenceとEventの生成
	/// </summary>
	void CreateFence();

	/// <summary>
	/// 深度Bufferの生成
	/// </summary>
	void CreateDepthBuffer();

	/// <summary>
	/// 各種ディスクリプタヒープの生成
	/// </summary>
	void CreateVariousDescriptorHeap();

	/// <summary>
	/// ディスクリプタヒープ
	/// </summary>
	void CreateDescriptorHeap();

	/// <summary>
	/// SwapChainからResource
	/// </summary>
	/// <param name="str"></param>
	/// <returns></returns>
	void GetResourcesFromSwapChain();

	/// <summary>
	/// RTVの作成
	/// </summary>
	void CreateRenderTargetViews();


	/// <summary>
	/// Fenceの生成
	/// </summary>
	void FenceGeneration();

	/// <summary>
	/// コマンドを積み込んで確定させる
	/// </summary>
	void SettleCommandList();

	/// <summary>
	/// TransitionBarrierを張る
	/// </summary>
	void SetupTransitionBarrier();

	/// <summary>
	/// RenderTargetの設定
	/// </summary>
	void RenderTargetPreference();

	/// <summary>
	/// コマンドリストのクローズ
	/// </summary>
	void CloseCommandList();

	/// <summary>
	/// コマンドのキック
	/// </summary>
	void ExecuteCommandList();

	/// <summary>
	/// 開放処理
	/// </summary>
	void ReleaseResources();

	/// <summary>
	/// リソースリークチェック
	/// </summary>
	void CheckResourceLeaks();

	///=====================================================/// 
	///生成関数
	///=====================================================///
	
	/// <summary>
	/// 深度BufferステンシルBufferの生成関数
	/// </summary>
	/// <param name="device"></param>
	/// <param name="width"></param>
	/// <param name="height"></param>
	/// <returns></returns>
	Microsoft::WRL::ComPtr <ID3D12Resource> CreateDepthStencilTextureResource(Microsoft::WRL::ComPtr <ID3D12Device> device, int32_t width, int32_t height);

	/// <summary>
	/// DescriptorHeap関数
	/// </summary>
	/// <param name="device"></param>
	/// <param name="heapType"></param>
	/// <param name="numDescriptors"></param>
	/// <param name="shaderVisible"></param>
	/// <returns></returns>
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> CreateDescriptorHeap(Microsoft::WRL::ComPtr <ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);
	

	///========================================================///
	///ゲッター・セッター
	///========================================================///
	void SetHr(HRESULT sHr) { this->hr = sHr; }

	HRESULT GetHr() { return hr; }

	void SetDevice(Microsoft::WRL::ComPtr <ID3D12Device> sDevice) { this->device_ = sDevice; }

	Microsoft::WRL::ComPtr <ID3D12Device> GetDevice() { return device_; }

	void SetCommandList(Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList> sCommandList) { this->commandList_ = sCommandList; }

	Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList> GetCommandList() { return commandList_.Get(); }

	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() { return swapChainDesc_; }

	D3D12_RENDER_TARGET_VIEW_DESC GetRtvDesc() { return rtvDesc_; }

	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> GetRtvDescriptorHeap() { return rtvDescriptorHeap_; }


	/// <summary>
	/// ログ確認
	/// </summary>
	/// <param name="message"></param>
	void Log(const std::string& message);

private:

	/// ===WindowsAPI=== ///
	WinApp* winApp_ = nullptr;

	/// ===デバックレイヤーの生成=== ///
	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController_;

	/// ===DXGIファクトリーの生成=== ///
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
	HRESULT hr;

	/// ===使用するアダプタ用変数=== ///
	Microsoft::WRL::ComPtr <IDXGIAdapter4> useAdapter_;

	/// ===D3D12Deviceの作成=== ///
	Microsoft::WRL::ComPtr <ID3D12Device> device_;
 
	/// ===エラー・警告の場合即停止(初期化完了のあとに行う)=== ///
	Microsoft::WRL::ComPtr <ID3D12InfoQueue> infoQueue_;

	//アダプターの情報を取得
	DXGI_ADAPTER_DESC3 adapterDesc_{};

	///========================================================///
	//コマンドキューを作成する
	///========================================================///
	Microsoft::WRL::ComPtr <ID3D12CommandQueue> commandQueue_;

	///========================================================///
	//コマンドアロケータを生成する
	///========================================================///
	Microsoft::WRL::ComPtr <ID3D12CommandAllocator> commandAllocator_;
	Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList> commandList_;

	///========================================================///
	//スワップチェーンを生成する
	///========================================================///
	Microsoft::WRL::ComPtr <IDXGISwapChain4> swapChain_;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};

	///========================================================///
	//Fenceの生成
	///========================================================///
	Microsoft::WRL::ComPtr <ID3D12Fence> fence_;
	uint64_t fenceValue_ = 0;
	HANDLE fenceEvent_;

	/// ===深度バッファ=== ///
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> dsvDescriptorHeap_;

	///========================================================///
	//ディスクリプタヒープ
	///========================================================///
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> rtvDescriptorHeap_;
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc_{};


	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};

	//ディスクリプタの先頭を取得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStarHandle_;
	//RTVを2つ作るのでディスクリプタを2つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];

	//これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex_;

	//TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier_{};

	///========================================================///
	//SwapChainからResource
	///========================================================///
	Microsoft::WRL::ComPtr <ID3D12Resource> swapChainResource_[2] = { nullptr };
};

