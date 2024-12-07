/*********************************************************************
 * \file   DirectXCore.cpp
 * \brief
 *
 * \author Harukichimaru
 * \date   November 2024
 * \note
 *********************************************************************/
#include "DirectXCore.h"
 //========================================
 // 標準ライブラリ
#include <vector>
//========================================
// DirectXTex
#include "d3dx12.h"
#pragma comment(lib,"winmm.lib")


///=============================================================================
///						描画前処理
//TODO:ループ内の前処理後処理を作成
void DirectXCore::PreDraw() {
	/// バックバッファの決定
	SettleCommandList();
	/// バリア設定
	SetupTransitionBarrier();
	// 描画ターゲットの設定とクリア
	RenderTargetPreference();

	// ViewPortとScissorRectの設定
	commandList_->RSSetViewports(1, &viewport_);
	commandList_->RSSetScissorRects(1, &scissorRect_);
}

///=============================================================================
///						描画後処理
void DirectXCore::PostDraw() {
	//FPS固定
	UpdateFixFPS();
	// コマンドリストのクローズと実行
	CloseCommandList();
	ExecuteCommandList();
}

///=============================================================================
///						DirectXの初期化
void DirectXCore::InitializeDirectX(WinApp* winApp) {
	//FPS固定初期化
	InitializeFixFPS();

	/// ===WinApp=== ///
	///NULL検出
	assert(winApp);
	/// メンバ変数に記録
	this->winApp_ = winApp;

	// ウィンドウハンドルの取得
	CreateDebugLayer();
	// DXGIファクトリーの生成
	CreateDxgiFactory();
	// アダプタの選択
	SelectAdapter();
	// D3D12デバイスの生成
	CreateD3D12Device();
	// コマンドキューの生成
	SetupErrorHandling();
	// コマンドキューの生成
	CreateCommandQueue();
	// コマンドアロケータの生成
	CreateCommandAllocator();
	// コマンドリストの生成
	CreateSwapChain();
	// フェンスの生成
	CreateFence();
	//深度バッファの生成
	CreateDepthBuffer();
	//様々なヒープサイズの取得
	CreateVariousDescriptorHeap();
	//RTVディスクリプタヒープの生成
	CreateRTVDescriptorHeap();
	//スワップチェーンからリソースを取得
	GetResourcesFromSwapChain();
	//RTVの生成
	CreateRenderTargetViews();
	//コマンドリストの決定
	SettleCommandList();
	//バリアの設定
	SetupTransitionBarrier();
	//DXCコンパイラーの初期化
	CreateDXCCompiler();
	//ビューポートとシザーレクトの生成
	CreateVirePortAndScissorRect();

	//=======================================
	//コマンドリストの設定
	CloseCommandList();
	//コマンドキック
	ExecuteCommandList();
	//フェンス生成
	FenceGeneration();
	//Imguiの初期化
	ImGuiInitialize();
}

///=============================================================================
///						開放処理
void DirectXCore::ReleaseDirectX() {
	///開放処理
	ReleaseResources();
}

///=============================================================================
///						デバックレイヤーの生成
void DirectXCore::CreateDebugLayer() {
#ifdef _DEBUG
	debugController_ = nullptr;
	if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_)))) {
		//デバックレイヤーを有効化する
		debugController_->EnableDebugLayer();
		//GPU側でもチェックを行うようにする
		debugController_->SetEnableGPUBasedValidation(TRUE);
	}
#endif
}

///=============================================================================
///						DXGIファクトリーの生成
void DirectXCore::CreateDxgiFactory() {
	dxgiFactory_ = nullptr;
	//HRESULTはWindows系のエラーコードであり、
	//開数が成功したかどうかをSUCCEEDマクロで判断できる
	hr_ = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	//初期化の根本的なエラーを判断するためassertにする
	assert(SUCCEEDED(hr_));
}

///=============================================================================
///						使用するアダプタ用変数
void DirectXCore::SelectAdapter() {
	useAdapter_ = nullptr;
	//良い順にアダプタを頼む
	for(UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) !=
		DXGI_ERROR_NOT_FOUND; i++) {
		//アダプターの情報を取得
		hr_ = useAdapter_->GetDesc3(&adapterDesc_);
		assert(SUCCEEDED(hr_));//取得不可
		//ソフトウェアアダプタでなければ採用
		if(!( adapterDesc_.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE )) {
			//採用したアダプタの情報をログに出力。Wstringの方に注意
			Log(ConvertString(std::format(L"Use Adapter;{}\n", adapterDesc_.Description)));
			break;
		}
		useAdapter_ = nullptr; //ソフトウェアアダプタの場合は見なかったことにできる
	}
	//適切なアダプタが見つからなかったので起動不可
	assert(useAdapter_ != nullptr);
}

///=============================================================================
///						D3D12Deviceの作成
void DirectXCore::CreateD3D12Device() {
	device_ = nullptr;
	//機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* feartureLevelStrings[] = { "12.2","12.1","12.0" };
	//高い順に生成できるか試してみる
	for(size_t i = 0; i < _countof(featureLevels); ++i) {
		//採用したアダプタでデバイスを作成
		hr_ = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
		//指定した機能レベルでデバイスが生成できたかを確認
		if(SUCCEEDED(hr_)) {
			//生成できたのでログ出力を行ってループを抜ける
			Log(std::format("ENGINE MESSAGE: FeatureLevel : {}\n", feartureLevelStrings[i]));
			break;
		}
	}


	//デバイスの生成がうまくいかなかったので起動できない
	assert(device_ != nullptr);
	//初期化完了のログの出力
	Log("ENGINE MESSAGE: Complete create D3D12Device!!!\n");
}

///=============================================================================
///						エラー・警告の場合即停止(初期化完了のあとに行う)
void DirectXCore::SetupErrorHandling() {
#ifdef  _DEBUG
	infoQueue_ = nullptr;
	if(SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue_)))) {
		///やべぇエラー時に停止
		infoQueue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		///エラー時に停止
		infoQueue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		///警告時に停止
		//NOTE:開放を忘れた場合、以下のコードをコメントアウトすること
		infoQueue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		/// ===特定のエラーの無視など=== ///
		//抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			//NOTE:Windows11でのDXGIデバックレイヤーとDX12デバックレイヤーの相互作用バグによるエラーメッセージ
			//https://stakoverflow.com/question/69805245/directx-12-application-is-crashing-in-windows-11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		//抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//指定したメッセージの表示を抑制する
		infoQueue_->PushStorageFilter(&filter);
	}
#endif //  _DEBUG
}


///=============================================================================
///						コマンドキューを作成する
void DirectXCore::CreateCommandQueue() {
	commandQueue_ = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr_ = device_->CreateCommandQueue(&commandQueueDesc,
		IID_PPV_ARGS(&commandQueue_));
	//コマンドキューの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr_));
}


///=============================================================================
///						コマンドアロケータを生成する
void DirectXCore::CreateCommandAllocator() {
	commandAllocator_ = nullptr;
	hr_ = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	//コマンドアロケータのせいせがうまくいかなかったので起動できない
	assert(SUCCEEDED(hr_));

	//コマンドリスト
	commandList_ = nullptr;
	hr_ = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr,
		IID_PPV_ARGS(&commandList_));
	//コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr_));
}


///=============================================================================
///						スワップチェーンを生成する
void DirectXCore::CreateSwapChain() {
	swapChain_ = nullptr;
	swapChainDesc_.Width = winApp_->GetWindowWidth();
	swapChainDesc_.Height = winApp_->GetWindowHeight();
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc_.SampleDesc.Count = 1;
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc_.BufferCount = 2;
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	//コマンドキュー、ウィンドウバレル、設定を渡して生成する
	hr_ = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_.Get(), winApp_->GetWindowHandle(), &swapChainDesc_, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>( swapChain_.GetAddressOf() ));
	assert(SUCCEEDED(hr_));
}

///=============================================================================
///						FenceとEventの生成
void DirectXCore::CreateFence() {
	//初期値0でFenceを作る
	fence_ = nullptr;
	fenceValue_ = 0;
	hr_ = device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr_));

	//FenceのSignalを持つためのイベントを生成する
	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(SUCCEEDED(hr_));
}

///=============================================================================
///						深度バッファの生成
void DirectXCore::CreateDepthBuffer() {
	/// ===DepthStencilTextureをウィンドウのサイズで作成=== ///
	depthStencilResource_ = CreateDepthStencilTextureResource(winApp_->GetWindowWidth(), winApp_->GetWindowHeight());
	/// ===dsv用DescriptorHeap=== ///
	dsvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
	/// ===dsvの設定=== ///
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//Format。基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2dTexture
	//DSVHeapの先頭にDSVを作る
	device_.Get()->CreateDepthStencilView(depthStencilResource_.Get(), &dsvDesc, dsvDescriptorHeap_.Get()->GetCPUDescriptorHandleForHeapStart());

	dsvHandle_ = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
}

///=============================================================================
///						各種ディスクリプタヒープの生成
void DirectXCore::CreateVariousDescriptorHeap() {
	/// ===DescriptorHeapのサイズを取得=== ///
	//descriptorSizeSRV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	descriptorSizeRTV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	descriptorSizeDSV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

///=============================================================================
///						RTVディスクリプタヒープ
void DirectXCore::CreateRTVDescriptorHeap() {
	//ディスクリプタヒープの生成
	rtvDescriptorHeap_ = nullptr;
	rtvDescriptorHeapDesc_.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescriptorHeapDesc_.NumDescriptors = 2;
	hr_ = device_->CreateDescriptorHeap(&rtvDescriptorHeapDesc_, IID_PPV_ARGS(&rtvDescriptorHeap_));

	assert(SUCCEEDED(hr_));
}

///=============================================================================
///						SwapChainからResource
void DirectXCore::GetResourcesFromSwapChain() {
	//SwapChainからResourceを引っ張ってくる
	hr_ = swapChain_->GetBuffer(0, IID_PPV_ARGS(&swapChainResource_[0]));
	//うまく取得できなければ起動できない
	assert(SUCCEEDED(hr_));
	hr_ = swapChain_->GetBuffer(1, IID_PPV_ARGS(&swapChainResource_[1]));
	assert(SUCCEEDED(hr_));
}


///=============================================================================
///						RTVの作成
void DirectXCore::CreateRenderTargetViews() {
	rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;		//出力結果をSRGBに変換して書き込む
	rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	//2dテクスチャとして書き込む
	//ディスクリプタの先頭を取得する
	rtvStarHandle_ = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	//RTVを2つ作るのでディスクリプタを2つ用意

	//１つ目の作成
	rtvHandles_[0] = rtvStarHandle_;
	device_->CreateRenderTargetView(swapChainResource_[0].Get(), &rtvDesc_, rtvHandles_[0]);
	//2つめのディスクリプハンドルの作成
	rtvHandles_[1].ptr = rtvHandles_[0].ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//2つめの作成
	device_->CreateRenderTargetView(swapChainResource_[1].Get(), &rtvDesc_, rtvHandles_[1]);
}


///=============================================================================
///						Fenceの生成
void DirectXCore::FenceGeneration() {
	fenceValue_++;
	//GPUがここまでたどり着いついたときに、Fenceの値を指定した値に代入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), fenceValue_);
	//GetCompketedvalueの初期値はFence作成時に渡した初期値
	if(fence_->GetCompletedValue() < fenceValue_) {
		//指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		//イベントを待つ
		WaitForSingleObject(fenceEvent_, INFINITE);
	}
}


///=============================================================================
///						コマンド積み込んで確定させる
void DirectXCore::SettleCommandList() {
	//これから書き込むバックバッファのインデックスを取得
	backBufferIndex_ = swapChain_->GetCurrentBackBufferIndex();
}


///=============================================================================
///						TransitionBarrierを張る
void DirectXCore::SetupTransitionBarrier() {
	//ここでのバリアはTransition
	barrier_.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにしておく
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier_.Transition.pResource = swapChainResource_[backBufferIndex_].Get();
	//遷移前の(現在)のResouceState
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	//遷移後のReosuceState
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier_);
}


///=============================================================================
///						RenderTargetの設定
void DirectXCore::RenderTargetPreference() {
	//描画先のRTVを設定する
	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex_], false, &dsvHandle_);
	//指定した色で画面全体をクリアする	
    float clearColor[] = { 0.298f, 0.424f, 0.702f, 1.0f }; // この色を変更することでウィンドウの色を#4c6cb3に変更できます
	commandList_->ClearRenderTargetView(rtvHandles_[backBufferIndex_], clearColor, 0, nullptr);
	commandList_->ClearDepthStencilView(dsvHandle_, D3D12_CLEAR_FLAG_DEPTH, 1.0F, 0, 0, nullptr);
}

///=============================================================================
///						ビューポートとシザーレクトの生成
void DirectXCore::CreateVirePortAndScissorRect() {
	//========================================
	// ビューポート
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport_.Width = static_cast<float>( winApp_->GetWindowWidth() );
	viewport_.Height = static_cast<float>( winApp_->GetWindowHeight() );
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0;
	viewport_.MaxDepth = 1.0f;
	//========================================
	// シザー矩形
	//基本的にビューポートと同じ矩形が構成されるようになる
	scissorRect_.left = 0;
	scissorRect_.right = winApp_->GetWindowWidth();
	scissorRect_.top = 0;
	scissorRect_.bottom = winApp_->GetWindowHeight();
}

///=============================================================================
///						コマンドリストの決定
void DirectXCore::CloseCommandList() {

	//画面に書く処理はすべて終わり。画面に映すので状態を遷移
	//今回はRenderTargetからPresebtにする
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	//TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier_);
	//コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
	hr_ = commandList_->Close();
	assert(SUCCEEDED(hr_));
}

///=============================================================================
///						コマンドのキック
void DirectXCore::ExecuteCommandList() {
	//GPUにコマンドリストの実行を行わせる
	Microsoft::WRL::ComPtr <ID3D12CommandList> commandLists[] = { commandList_ };
	commandQueue_->ExecuteCommandLists(1, commandLists->GetAddressOf());
	//GPUとOSに画面の交換を行うように通知する
	swapChain_->Present(1, 0);

	fenceValue_++;
	//GPUがここまでたどり着いついたときに、Fenceの値を指定した値に代入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), fenceValue_);
	//GetCompketedvalueの初期値はFence作成時に渡した初期値
	if(fence_->GetCompletedValue() < fenceValue_) {
		//指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		//イベントを待つ
		WaitForSingleObject(fenceEvent_, INFINITE);
	}

	//次フレーム用のコマンドリストを準備
	hr_ = commandAllocator_->Reset();
	assert(SUCCEEDED(hr_));
	hr_ = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr_));
}

///=============================================================================
///						開放処理
void DirectXCore::ReleaseResources() {
	CloseHandle(fenceEvent_);
#ifdef _DEBUG
	//debugController_->Release();
#endif // DEBUG
	CloseWindow(winApp_->GetWindowHandle());
}

///=============================================================================
///						リソースリークチェック
void DirectXCore::CheckResourceLeaks() {
	Microsoft::WRL::ComPtr <IDXGIDebug> debug;
	if(SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
		//開放を忘れてエラーが出た場合、205行目をコメントアウト
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
	}
}

///=============================================================================
///						ImGuiの初期化
void DirectXCore::ImGuiInitialize() {
	//IMGUI_CHECKVERSION();
	//ImGui::CreateContext();
	//ImGui::StyleColorsDark();
	//ImGui_ImplWin32_Init(winApp_->GetWindowHandle());
	//ImGui_ImplDX12_Init(device_.Get(),
	//	swapChainDesc_.BufferCount,
	//	rtvDesc_.Format,
	//	srvDescriptorHeap_.Get(),
	//	srvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart(),
	//	srvDescriptorHeap_->GetGPUDescriptorHandleForHeapStart());
}

///=============================================================================
///						DXCコンパイラーの初期化
void DirectXCore::CreateDXCCompiler() {
	//dxcCompilerを初期化
	hr_ = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr_));
	hr_ = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr_));
	//現時点でincludeはしないが、includeに対応するために設定を行う
	hr_ = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr_));
}

///=============================================================================
///						深度BufferステンシルBufferの生成関数
Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCore::CreateDepthStencilTextureResource(int32_t width, int32_t height) {
	/// ===生成するResouceの設定=== ///
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;										//テクスチャの幅
	resourceDesc.Height = height;									//テクスチャの高さ
	resourceDesc.MipLevels = 1;										//mipmapの数
	resourceDesc.DepthOrArraySize = 1;								//奥行きor配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;			//DepthStencillとして利用可能なFormat
	resourceDesc.SampleDesc.Count = 1;								//サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	//2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;	//DepthStencillとして使う通知

	/// ===利用するHeapの設定=== ///
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//VRAM上に作る

	/// ===深度値のクリア設定=== ///
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;//1.0F(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//フォーマット。Resourceと合わせる


	/// ===設定を元にResourceの生成を行う=== ///
	Microsoft::WRL::ComPtr <ID3D12Resource> resource = nullptr;
	HRESULT hr = device_->CreateCommittedResource(
		&heapProperties,					//Heapの設定
		D3D12_HEAP_FLAG_NONE,				//heepの特殊な設定。特になし。
		&resourceDesc,						//Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	//深度値を書き込む状態にしておく
		&depthClearValue,					//Clear最適値
		IID_PPV_ARGS(&resource));			//作成するResourceポインタへのポインタ

	//========================================
	// エラーチェック
#ifdef _DEBUG
	assert(SUCCEEDED(hr));
#endif // _DEBUG
	if(FAILED(hr)) {
		//深度ステンシルテクスチャの生成がうまくいかなかったので起動できない
		Log("Failed to create depth stencil texture resource.");
		return nullptr;
	}

	//========================================
	// 出力
	return resource;
}

///=============================================================================
///						DescriptorHeapの生成関数
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DirectXCore::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	// NOTE:debugモードでのみエラーチェックを行う理由は、assertを使っているため
	HRESULT hr = device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
#ifdef _DEBUG
	assert(SUCCEEDED(hr));
#endif // _DEBUG
	//
	if(FAILED(hr)) {
		// ディスクリプタヒープの生成がうまくいかなかったので起動できない
		Log("Failed to Create Descriptor Heap.");
		return nullptr;
	}
	// 成功したログを出力
	Log("Descriptor heap created successfully.");
	return descriptorHeap;
}

///=============================================================================
///						シェーダーのコンパイル
IDxcBlob* DirectXCore::CompileShader(const std::wstring& filePath, const wchar_t* profile) {

	/// ===hlseファイルを読む=== ///
	//これからシェーダーをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin Compiler,path:{},profile:{}\n", filePath, profile)));
	//hlseファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	//読めなかったら止める
	assert(SUCCEEDED(hr));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer = {};
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;//UTF-8の文字コードであることを通知

	/// ===コンパイルする=== ///
	LPCWSTR arguments[] = {
	filePath.c_str(),
	L"-E",L"main",
	L"-T",profile,
	L"-Zi",L"-Qembed_debug",
	L"-Od",
	L"-Zpr",
	};
	//実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler_->Compile(
		&shaderSourceBuffer,
		arguments,
		_countof(arguments),
		includeHandler_,
		IID_PPV_ARGS(&shaderResult)
	);
	//コンパイルエラーではなくdxcが起動できないと致命的な状況
	assert(SUCCEEDED(hr));

	/// ===警告・エラーがでてないか確認する=== ///
	IDxcBlobUtf8* shaderError = nullptr;
	if(shaderResult->HasOutput(DXC_OUT_ERRORS)) {
		shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	}
	//エラーがある場合はエラーを出力して終了
	if(shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());
		if(shaderResult->HasOutput(DXC_OUT_ERRORS)) {
			shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
		}
		//警告・エラーダメ絶対
		assert(false);
	}

	/// ===Compile結果を受け取って返す=== ///
	//コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	//成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeeded, path:{},profile:{}\n", filePath, profile)));
	//もう使わないリソースを開放
	shaderSource->Release();
	shaderResult->Release();
	//実行用のバイナリを返却
	return shaderBlob;
}

///=============================================================================
///						バッファーリソースの生成
Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCore::CreateBufferResource(size_t sizeInByte) { 		// バッファリソースの設定を作成
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = sizeInByte;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// アップロードヒープのプロパティを設定
	//頂点リソース用のヒープ設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

	// リソースを作成
	Microsoft::WRL::ComPtr <ID3D12Resource> resource = nullptr;
	HRESULT hr = device_->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource)
	);

	// エラーチェック
	if(FAILED(hr) || !resource) {
		// リソースの作成に失敗した場合、エラーメッセージを出力して nullptr を返す
		return nullptr;
	}

	return resource;
}

///=============================================================================
///						テクスチャリソースの生成
Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCore::CreateTextureResource(const DirectX::TexMetadata& metadata) {
	/// ===1.metadataを元にResouceの設定=== ///
	D3D12_RESOURCE_DESC resouceDesc{};
	resouceDesc.Width = UINT(metadata.width);								//Textureの幅
	resouceDesc.Height = UINT(metadata.height);								//Textureの高さ
	resouceDesc.MipLevels = UINT16(metadata.mipLevels);						//mipmapの数
	resouceDesc.DepthOrArraySize = UINT16(metadata.arraySize);				//奥行き or 配列Textureの配列数
	resouceDesc.Format = metadata.format;									//TextureのFormat
	resouceDesc.SampleDesc.Count = 1;										//サンプリングカウント
	resouceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);	//Textureの次元数。普段つかているのは2次元。

	/// ===2.利用するHeapの設定===///
	//TODO:リソースの場所を変更する03_00_ex
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//細かい設定を行う

	/// ===3.resouceを生成する=== ///
	Microsoft::WRL::ComPtr <ID3D12Resource> resource = nullptr;
	HRESULT hr = device_->CreateCommittedResource(
		&heapProperties,					//Heapの設定
		D3D12_HEAP_FLAG_NONE,				//Heapの特殊な設定、特になし
		&resouceDesc,						//Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST,	//初回のResouceState。Textureは基本読むだけ
		nullptr,
		IID_PPV_ARGS(&resource)
	);

	//========================================
	// エラーチェック
#ifdef _DEBUG
	assert(SUCCEEDED(hr));
#endif // _DEBUG
	if(FAILED(hr)) {
		//深度ステンシルテクスチャの生成がうまくいかなかったので起動できない
		Log("Failed to create depth stencil texture resource.");
		return nullptr;
	}

	//========================================
	// 出力
	return resource;
}

///=============================================================================
///						テクスチャデータの転送
// NOTE:以下の手順を行う
//3.CPUで書き込む用にUploadHeapnnoResourceを作成
//4.3に対してCPUでデータを書き込む
//5.CommandListに3を2に転送するコマンドを積む
//NOTE:以下の文字は属性というもの。戻り値を破棄してはならないことを示す。
[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCore::UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages) {
	///----------------中間リソースの作成----------------///
	std::vector<D3D12_SUBRESOURCE_DATA> subresource;
	DirectX::PrepareUpload(device_.Get(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresource);
	//Subresourceの数を元に、コピー元となるintermediateResourceに必要なサイズを計算する
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresource.size()));
	//計算したサイズでintermediateResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(intermediateSize);

	///----------------データ転送をコマンドに積む----------------///
	//interにsubreのデータを書き込み、textureに転送する
	UpdateSubresources(commandList_.Get(), texture.Get(), intermediateResource.Get(), 0, 0, UINT(subresource.size()), subresource.data());

	///----------------読み込み変更コマンド----------------///
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList_->ResourceBarrier(1, &barrier);
	return intermediateResource;
}

///=============================================================================
///						DXTecを使ってファイルを読む
DirectX::ScratchImage DirectXCore::LoadTexture(const std::string& filePath) {
	/// ===テクスチャファイルを読んでプログラムを扱えるようにする=== ///
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	/// ===ミニマップの作成=== ///
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	/// ===ミニマップ付きのデータを返す=== ///
	return mipImages;
}


///=============================================================================
///						DescriptorHandleの取得を関数化
///--------------------------------------------------------------
///						 CPU
D3D12_CPU_DESCRIPTOR_HANDLE DirectXCore::GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	// NOTE:サブ式よりも先に乗算を行うため、括弧をつける
	handleCPU.ptr += ( static_cast<unsigned long long>( descriptorSize ) * index );
	return handleCPU;
}

///--------------------------------------------------------------
///						 GPU
D3D12_GPU_DESCRIPTOR_HANDLE DirectXCore::GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	// NOTE:サブ式よりも先に乗算を行うため、括弧をつける
	handleGPU.ptr += ( static_cast<unsigned long long>( descriptorSize ) * index );
	return handleGPU;
}


///=============================================================================
///						60FPS固定の処理
///--------------------------------------------------------------
///						 InitializeFixFPS
void DirectXCore::InitializeFixFPS() {
	//システムタイマーの分解能を上げる
	timeBeginPeriod(1);
	//現在時間を記録する
	reference_ = std::chrono::steady_clock::now();
}

///--------------------------------------------------------------
///						 UpdateFixFPS
void DirectXCore::UpdateFixFPS() {
	// 1/60秒ピッタリの時間
	const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));
	// 1/60秒よりわずかに短い時間
	const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 65.0f));

	//現在時間を取得する
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	std::chrono::microseconds elapsed =
		std::chrono::duration_cast<std::chrono::microseconds>( now - reference_ );

	// 1/60秒(よりわずかに短い時間)経っていない場合
	if(elapsed < kMinCheckTime) {
		//1/60秒経過するまで軽微なスリープを繰り返す
		while(std::chrono::steady_clock::now() - reference_ < kMinTime) {
			//1マイクロ秒スリープ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}

	//現在の時間を記録する
	reference_ = std::chrono::steady_clock::now();
}


