/*********************************************************************
 * \file   DirectXCore.h
 * \brief
 *
 * \author Harukichimaru
 * \date   November 2024
 * \note
 *********************************************************************/
#pragma once
 //========================================
 // 標準ライブラリ
#include <cstdint>
#include <string>
#include <format>
#include <cassert>
#include <wrl.h>
#include <chrono>
#include <thread>
//========================================
// 自作関数
#include "WstringUtility.h"
using namespace WstringUtility;
#include "Logger.h"
using namespace Logger;
#include "WinApp.h"
//========================================
// ReportLiveObj
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
//========================================
// DX12 include
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
//========================================
// DXC
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")
// DXtec
#include "DirectXTex.h"
//========================================
// imgui
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
///=============================================================================
///						クラス
class DirectXCore {
public:
	///--------------------------------------------------------------
	///						 メンバ関数

	/**----------------------------------------------------------------------------
	 * \brief  PreDraw ループ前処理
	 * \note   
	 */
	void PreDraw();

	/**----------------------------------------------------------------------------
	 * \brief  PostDraw ループ後処理
	 * \note   
	 */
	void PostDraw();

	/**----------------------------------------------------------------------------
	 * \brief  InitializeDirectX ダイレクトXの初期化
	 * \param  winApp
	 * \note   
	 */
	void InitializeDirectX(WinApp* winApp);

	/**----------------------------------------------------------------------------
	 * \brief  ReleaseDirectX ダイレクトXの開放
	 * \note   
	 */
	void ReleaseDirectX();

	/**----------------------------------------------------------------------------
	 * \brief  CreateDebugLayer デバッグレイヤーの生成
	 * \note   
	 */
	void CreateDebugLayer();

	/**----------------------------------------------------------------------------
	 * \brief  CreateDxgiFactory DXGIファクトリーの生成
	 * \note   
	 */
	void CreateDxgiFactory();

	/**----------------------------------------------------------------------------
	 * \brief  SelectAdapter アダプタの選択
	 * \note   
	 */
	void SelectAdapter();

	/**----------------------------------------------------------------------------
	 * \brief  CreateD3D12Device デバイスの生成
	 * \note   
	 */
	void CreateD3D12Device();

	/**----------------------------------------------------------------------------
	 * \brief  SetupErrorHandling エラーハンドリングの設定
	 * \note   
	 */
	void SetupErrorHandling();

	/**----------------------------------------------------------------------------
	 * \brief  CreateCommandQueue コマンドキューの生成
	 * \note   
	 */
	void CreateCommandQueue();

	/**----------------------------------------------------------------------------
	 * \brief  CreateCommandAllocator コマンドアロケータの生成
	 * \note   
	 */
	void CreateCommandAllocator();

	/**----------------------------------------------------------------------------
	 * \brief  CreateSwapChain SwapChainの生成
	 * \note   
	 */
	void CreateSwapChain();

	/**----------------------------------------------------------------------------
	 * \brief  CreateFence FenceとEventの生成
	 * \note   
	 */
	void CreateFence();

	/**----------------------------------------------------------------------------
	 * \brief  CreateDepthBuffer 深度Bufferの生成
	 * \note   
	 */
	void CreateDepthBuffer();

	/**----------------------------------------------------------------------------
	 * \brief  CreateVariousDescriptorHeap ディスクリプタヒープの生成
	 * \note   
	 */
	void CreateVariousDescriptorHeap();

	/**----------------------------------------------------------------------------
	 * \brief  CreateSRVDescriptorHeap SRVディスクリプタヒープ
	 * \note   
	 */
	void CreateSRVDescriptorHeap();

	/**----------------------------------------------------------------------------
	 * \brief  CreateRTVDescriptorHeap RTVディスクリプタヒープ
	 * \note   
	 */
	void CreateRTVDescriptorHeap();

	/**----------------------------------------------------------------------------
	 * \brief  GetResourcesFromSwapChain SwapChainからResource
	 * \note   
	 */
	void GetResourcesFromSwapChain();

	/**----------------------------------------------------------------------------
	 * \brief  CreateRenderTargetViews RTVの作成
	 * \note   
	 */
	void CreateRenderTargetViews();

	/**----------------------------------------------------------------------------
	 * \brief  FenceGeneration フェンスの生成
	 * \note   
	 */
	void FenceGeneration();

	/**----------------------------------------------------------------------------
	 * \brief  SettleCommandList コマンドリストの決定
	 * \note   
	 */
	void SettleCommandList();

	/**----------------------------------------------------------------------------
	 * \brief  SetupTransitionBarrier TransitionBarrierの設定
	 * \note   
	 */
	void SetupTransitionBarrier();

	/**----------------------------------------------------------------------------
	 * \brief  RenderTargetPreference レンダーターゲットの設定
	 * \note   
	 */
	void RenderTargetPreference();

	/**----------------------------------------------------------------------------
	 * \brief  CreateVirePortAndScissorRect ビューポートとシザーレクトの生成
	 * \note   
	 */
	void CreateVirePortAndScissorRect();

	/**----------------------------------------------------------------------------
	 * \brief  CloseCommandList コマンドリストの終了
	 * \note   
	 */
	void CloseCommandList();

	/**----------------------------------------------------------------------------
	 * \brief  ExecuteCommandList コマンドリストの実行
	 * \note   
	 */
	void ExecuteCommandList();

	/**----------------------------------------------------------------------------
	 * \brief  ReleaseResources リソースの開放
	 * \note   
	 */
	void ReleaseResources();

	/**----------------------------------------------------------------------------
	 * \brief  CheckResourceLeaks リソースリークのチェック
	 * \note   
	 */
	void CheckResourceLeaks();

	/**----------------------------------------------------------------------------
	 * \brief  ImGuiInitialize ImGuiの初期化
	 * \note   
	 */
	void ImGuiInitialize();

	/**----------------------------------------------------------------------------
	 * \brief  CreateDXCCompiler DXCコンパイラーの初期化
	 * \note   
	 */
	void CreateDXCCompiler();


	///--------------------------------------------------------------
	///						 生成系メンバ関数
	/**----------------------------------------------------------------------------
	 * \brief  CreateDepthStencilTextureResource 深度BufferステンシルBufferの生成
	 * \param  width
	 * \param  height
	 * \return
	 * \note
	 */
	Microsoft::WRL::ComPtr <ID3D12Resource> CreateDepthStencilTextureResource(int32_t width, int32_t height);

	/**----------------------------------------------------------------------------
	 * \brief  CreateDescriptorHeap ディスクリプタヒープの生成
	 * \param  heapType ヒープタイプ
	 * \param  numDescriptors ディスクリプタ数
	 * \param  shaderVisible シェーダーから見えるか
	 * \return
	 * \note
	 */
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	// TODO:08-02で消去
	/**----------------------------------------------------------------------------
	 * \brief  GetSRVCPUDescriptorHandle SRVの指定番号のCPUディスクリプタ‐ハンドルを取得
	 * \param  index インデックス
	 * \return
	 * \note
	 */
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index);
	// TODO:08-02で消去
	/**----------------------------------------------------------------------------
	 * \brief  GetSRVGPUDescriptorHandle SRVの指定番号のGPUディスクリプタ‐ハンドルを取得
	 * \param  index インデックス
	 * \return
	 * \note
	 */
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle(uint32_t index);

	/**----------------------------------------------------------------------------
	 * \brief  CompileShader シェーダーのコンパイル
	 * \param  filePath ファイルパス
	 * \param  profile プロファイル
	 * \return
	 * \note
	 */
	IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile);

	/**----------------------------------------------------------------------------
	 * \brief  CreateBufferResource バッファリソースの生成
	 * \param  sizeInByte サイズ
	 * \return
	 * \note
	 */
	Microsoft::WRL::ComPtr <ID3D12Resource> CreateBufferResource(size_t sizeInByte);

	/**----------------------------------------------------------------------------
	 * \brief  CreateTextureResource テクスチャリソースの生成
	 * \param  metadata メタデータ
	 * \return
	 * \note
	 */
	Microsoft::WRL::ComPtr <ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);

	/**----------------------------------------------------------------------------
	 * \brief  UploadTextureData テクスチャデータのアップロード
	 * \param  texture テクスチャ
	 * \param  mipImages ミップマップ
	 * \return
	 * \note
	 */
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(Microsoft::WRL::ComPtr <ID3D12Resource> texture, const DirectX::ScratchImage& mipImages);

	/**----------------------------------------------------------------------------
	 * \brief  LoadTexture テクスチャの読み込み
	 * \param  filePath ファイルパス
	 * \return
	 * \note
	 */
	static DirectX::ScratchImage LoadTexture(const std::string& filePath);


	///--------------------------------------------------------------
	///						 
private:
	/// ===CPU=== ///
	/**----------------------------------------------------------------------------
	 * \brief  GetCPUDescriptorHandle CPUディスクリプタハンドルの取得
	 * \param  descriptorHeap ディスクリプタヒープ
	 * \param  descriptorSize
	 * \param  index
	 * \return
	 * \note
	 */
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);

	/// ===GPU=== ///
	/**----------------------------------------------------------------------------
	 * \brief  GetGPUDescriptorHandle GPUディスクリプタハンドルの取得
	 * \param  descriptorHeap
	 * \param  descriptorSize
	 * \param  index
	 * \return
	 * \note
	 */
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);

	/**----------------------------------------------------------------------------
	 * \brief  InitializeFixFPS FPS固定更新の初期化
	 * \note
	 */
	void InitializeFixFPS();

	/**----------------------------------------------------------------------------
	 * \brief  UpdateFixFPS FPS固定更新
	 * \note
	 */
	void UpdateFixFPS();


	///--------------------------------------------------------------
	///						 入出力関数
public:

	/**----------------------------------------------------------------------------
	 * \brief  GetWinApp WindowsAPI取得
	 * \return
	 * \note
	 */
	WinApp GetWinApp() { return *winApp_; }

	/**----------------------------------------------------------------------------
	 * \brief  SetHr HRESULT型の変数を設定するセッター関数。
	 * \param  sHr HRESULT型の変数
	 * \note
	 */
	void SetHr(HRESULT sHr) { this->hr_ = sHr; }

	/**----------------------------------------------------------------------------
	 * \brief  GetHr HRESULT型の変数を取得するゲッター関数。
	 * \return
	 * \note
	 */
	HRESULT GetHr() const { return hr_; }

	/**----------------------------------------------------------------------------
	 * \brief  SetDevice デバイスの設定
	 * \param  sDevice デバイス
	 * \note
	 */
	void SetDevice(Microsoft::WRL::ComPtr <ID3D12Device> sDevice) { this->device_ = sDevice; }

	/**----------------------------------------------------------------------------
	 * \brief  GetDevice デバイスの取得
	 * \return
	 * \note
	 */
	Microsoft::WRL::ComPtr <ID3D12Device> GetDevice() { return device_; }

	/**----------------------------------------------------------------------------
	 * \brief  SetCommandList コマンドリストの設定
	 * \param  sCommandList
	 * \note
	 */
	void SetCommandList(Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList> sCommandList) { this->commandList_ = sCommandList; }

	/**----------------------------------------------------------------------------
	 * \brief  GetCommandList コマンドリストの取得
	 * \return
	 * \note
	 */
	Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList> GetCommandList() { return commandList_.Get(); }

	/**----------------------------------------------------------------------------
	 * \brief  GetSwapChainDesc スワップチェーンの設定の取得
	 * \return
	 * \note
	 */
	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() const { return swapChainDesc_; }

	/**----------------------------------------------------------------------------
	 * \brief  GetRtvDesc RTVディスクリプタの取得
	 * \return
	 * \note
	 */
	D3D12_RENDER_TARGET_VIEW_DESC GetRtvDesc() const { return rtvDesc_; }

	/**----------------------------------------------------------------------------
	 * \brief  GetRtvDescriptorHeap RTVディスクリプタヒープの取得
	 * \return
	 * \note
	 */
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> GetRtvDescriptorHeap() { return rtvDescriptorHeap_; }

	/**----------------------------------------------------------------------------
	 * \brief  GetSrvDescriptorHeap SRVディスクリプタヒープの取得
	 * \return
	 * \note
	 */
	//Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> GetSrvDescriptorHeap() { return srvDescriptorHeap_.Get(); }

	///----------------GetDescriptorSizeSRV----------------///
	//uint32_t GetDescriptorSizeSRV() const { return descriptorSizeSRV; }

	///--------------------------------------------------------------
	///						 メンバ変数
private:
//========================================
// 記録時間(FPS固定用)
std::chrono::steady_clock::time_point reference_;

//========================================
// WindowsAPI
WinApp* winApp_ = nullptr;

//========================================
// デバックレイヤーの生成
Microsoft::WRL::ComPtr<ID3D12Debug1> debugController_;

//========================================
// DXGIファクトリーの生成
Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
HRESULT hr_ = S_OK;  // Initialize HRESULT to S_OK

//========================================
// 使用するアダプタ用変数
Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_;

//========================================
// D3D12Deviceの作成
Microsoft::WRL::ComPtr<ID3D12Device> device_;

//========================================
// エラー・警告の場合即停止(初期化完了のあとに行う)
Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue_;
//アダプターの情報を取得
DXGI_ADAPTER_DESC3 adapterDesc_{};

//========================================
// コマンドキューを作成する
Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;

//========================================
// コマンドアロケータを生成する
Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;

//========================================
// スワップチェーンを生成する
Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};

//========================================
// Fenceの生成
Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
uint64_t fenceValue_ = 0;
HANDLE fenceEvent_ = nullptr;  // Initialize to nullptr

//========================================
// 深度バッファ
D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_{};
Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_;

//========================================
// DescriptorHeapサイズ
//uint32_t descriptorSizeSRV = 0;  // SRV
uint32_t descriptorSizeRTV = 0;  // RTV
uint32_t descriptorSizeDSV = 0;  // DSV

//========================================
// SRVディスクリプタヒープ
//Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_;

//========================================
// RTVディスクリプタヒープ
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_;
D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc_{};

D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};

//ディスクリプタの先頭を取得する
D3D12_CPU_DESCRIPTOR_HANDLE rtvStarHandle_{};
//RTVを2つ作るのでディスクリプタを2つ用意
D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2]{};

//これから書き込むバックバッファのインデックスを取得
UINT backBufferIndex_ = 0;

//TransitionBarrierの設定
D3D12_RESOURCE_BARRIER barrier_{};

//========================================
// SwapChainからResource
Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResource_[2] = { nullptr, nullptr };

//========================================
// DXCコンパイラ
IDxcUtils* dxcUtils_ = nullptr;
IDxcCompiler3* dxcCompiler_ = nullptr;
IDxcIncludeHandler* includeHandler_ = nullptr;

//========================================
// ビューポート
D3D12_VIEWPORT viewport_{};

//========================================
// シザー矩形
D3D12_RECT scissorRect_{};

//========================================
// テクスチャマネージャ関係
public:
//最大SRV数(最大テクスチャ枚数)
//static const uint32_t kMaxSRVCount_ = 512;
};

