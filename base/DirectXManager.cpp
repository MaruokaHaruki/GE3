#include "DirectXManager.h"

///=====================================================/// 
///コンストラクタ
///=====================================================/// 
DirectXManager::DirectXManager() {

}

///=====================================================/// 
///デストラクタ
///=====================================================/// 
DirectXManager::~DirectXManager() {
}

///=====================================================/// 
///描画前処理
///=====================================================///
//TODO:ループ内の前処理後処理を作成
void DirectXManager::PreDraw() {
	/// バックバッファの決定
	SettleCommandList();
	/// バリア設定
	SetupTransitionBarrier();
	// 描画ターゲットの設定とクリア
	RenderTargetPreference();
}

///=====================================================/// 
///描画後処理
///=====================================================///
void DirectXManager::PostDraw() {
	// コマンドリストのクローズと実行
	CloseCommandList();
	ExecuteCommandList();
}

///=====================================================/// 
///DirectXの初期化
///=====================================================/// 
void DirectXManager::InitializeDirectX(WinApp* winApp) {
	/// ===WinApp=== ///
	///NULL検出
	assert(winApp);
	/// メンバ変数に記録
	this->winApp_ = winApp;

	CreateDebugLayer();

	CreateDxgiFactory();

	SelectAdapter();

	CreateD3D12Device();

	SetupErrorHandling();

	CreateCommandQueue();

	CreateCommandAllocator();

	CreateSwapChain();

	CreateFence();

	//深度バッファの生成
	CreateDepthBuffer();

	CreateDescriptorHeap();

	GetResourcesFromSwapChain();

	CreateRenderTargetViews();

	SettleCommandList();

	SetupTransitionBarrier();

	//RenderTargetPreference();
}

///=====================================================/// 
///開放処理
///=====================================================/// 
void DirectXManager::ReleaseDirectX() {
	///開放処理
	ReleaseResources();
}

///=====================================================/// 
///ログの確認
///=====================================================/// 
void DirectXManager::Log(const std::string& message) {
	OutputDebugStringA(message.c_str());
}

///=====================================================/// 
///デバックレイヤーの生成
///=====================================================/// 
void DirectXManager::CreateDebugLayer() {
#ifdef _DEBUG
	debugController_ = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_)))) {
		//デバックレイヤーを有効化する
		debugController_->EnableDebugLayer();
		//GPU側でもチェックを行うようにする
		debugController_->SetEnableGPUBasedValidation(TRUE);
	}
#endif
}

///=====================================================/// 
///DXGIファクトリーの生成
///=====================================================/// 
void DirectXManager::CreateDxgiFactory() {
	dxgiFactory_ = nullptr;
	//HRESULTはWindows系のエラーコードであり、
	//開数が成功したかどうかをSUCCEEDマクロで判断できる
	hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	//初期化の根本的なエラーを判断するためassertにする
	assert(SUCCEEDED(hr));
}

///=====================================================/// 
///使用するアダプタ用変数
///=====================================================/// 
void DirectXManager::SelectAdapter() {
	useAdapter_ = nullptr;
	//良い順にアダプタを頼む
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) !=
		DXGI_ERROR_NOT_FOUND; i++) {
		//アダプターの情報を取得
		hr = useAdapter_->GetDesc3(&adapterDesc_);
		assert(SUCCEEDED(hr));//取得不可
		//ソフトウェアアダプタでなければ採用
		if (!( adapterDesc_.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE )) {
			//採用したアダプタの情報をログに出力。Wstringの方に注意
			Log(ConvertString(std::format(L"Use Adapter;{}\n", adapterDesc_.Description)));
			break;
		}
		useAdapter_ = nullptr; //ソフトウェアアダプタの場合は見なかったことにできる
	}
	//適切なアダプタが見つからなかったので起動不可
	assert(useAdapter_ != nullptr);
}

///=====================================================/// 
///D3D12Deviceの作成
///=====================================================/// 
void DirectXManager::CreateD3D12Device() {
	device_ = nullptr;
	//機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* feartureLevelStrings[] = { "12.2","12.1","12.0" };
	//高い順に生成できるか試してみる
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		//採用したアダプタでデバイスを作成
		hr = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
		//指定した機能レベルでデバイスが生成できたかを確認
		if (SUCCEEDED(hr)) {
			//生成できたのでログ出力を行ってループを抜ける
			Log(std::format("FeatureLevel : {}\n", feartureLevelStrings[i]));
			break;
		}
	}


	//デバイスの生成がうまくいかなかったので起動できない
	assert(device_ != nullptr);
	//初期化完了のログの出力
	Log("Complete create D3D12Device!!!\n");
}

///=====================================================/// 
///エラー・警告の場合即停止(初期化完了のあとに行う)
///=====================================================/// 
void DirectXManager::SetupErrorHandling() {
#ifdef  _DEBUG
	infoQueue_ = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue_)))) {
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


///=====================================================/// 
///コマンドキューを作成する
///=====================================================/// 
void DirectXManager::CreateCommandQueue() {
	commandQueue_ = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device_->CreateCommandQueue(&commandQueueDesc,
		IID_PPV_ARGS(&commandQueue_));
	//コマンドキューの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));
}


///=====================================================/// 
///コマンドアロケータを生成する
///=====================================================/// 
void DirectXManager::CreateCommandAllocator() {
	commandAllocator_ = nullptr;
	hr = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	//コマンドアロケータのせいせがうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	//コマンドリスト
	commandList_ = nullptr;
	hr = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr,
		IID_PPV_ARGS(&commandList_));
	//コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));
}


///=====================================================/// 
///スワップチェーンを生成する
///=====================================================/// 
void DirectXManager::CreateSwapChain() {
	swapChain_ = nullptr;
	swapChainDesc_.Width = winApp_->GetWindowWidth();
	swapChainDesc_.Height = winApp_->GetWindowHeight();
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc_.SampleDesc.Count = 1;
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc_.BufferCount = 2;
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	//コマンドキュー、ウィンドウバレル、設定を渡して生成する
	hr = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_.Get(), winApp_->GetWindowHandle(), &swapChainDesc_, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>( swapChain_.GetAddressOf() ));
	assert(SUCCEEDED(hr));
}


///=====================================================/// 
///FenceとEventの生成
///=====================================================/// 
void DirectXManager::CreateFence() {
	//初期値0でFenceを作る
	fence_ = nullptr;
	fenceValue_ = 0;
	hr = device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr));

	//FenceのSignalを持つためのイベントを生成する
	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(SUCCEEDED(hr));
}

///=====================================================/// 
///深度バッファの生成
///=====================================================///
void DirectXManager::CreateDepthBuffer() {
	/// ===DepthStencilTextureをウィンドウのサイズで作成=== ///
	depthStencilResource_ = CreateDepthStencilTextureResource(device_.Get(), winApp_->GetWindowWidth(), winApp_->GetWindowHeight());
	/// ===dsv用DescriptorHeap=== ///
	dsvDescriptorHeap_ = CreateDescriptorHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
	/// ===dsvの設定=== ///
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//Format。基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2dTexture
	//DSVHeapの先頭にDSVを作る
	device_.Get()->CreateDepthStencilView(depthStencilResource_.Get(), &dsvDesc, dsvDescriptorHeap_.Get()->GetCPUDescriptorHandleForHeapStart());

	dsvHandle_ = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
}

///=====================================================/// 
///深度バッファの生成
///=====================================================///
void DirectXManager::CreateVariousDescriptorHeap() {
	/// ===DescriptorHeapのサイズを取得=== ///

	const uint32_t descriptorSizeSRV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	const uint32_t descriptorSizeRTV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	const uint32_t descriptorSizeDSV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);



}


///=====================================================/// 
///ディスクリプタヒープ
///=====================================================/// 
void DirectXManager::CreateDescriptorHeap() {
	//ディスクリプタヒープの生成
	rtvDescriptorHeap_ = nullptr;
	rtvDescriptorHeapDesc_.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescriptorHeapDesc_.NumDescriptors = 2;
	hr = device_->CreateDescriptorHeap(&rtvDescriptorHeapDesc_, IID_PPV_ARGS(&rtvDescriptorHeap_));

	assert(SUCCEEDED(hr));
}


///=====================================================/// 
///SwapChainからResource
///=====================================================/// 
void DirectXManager::GetResourcesFromSwapChain() {
	//SwapChainからResourceを引っ張ってくる
	hr = swapChain_->GetBuffer(0, IID_PPV_ARGS(&swapChainResource_[0]));
	//うまく取得できなければ起動できない
	assert(SUCCEEDED(hr));
	hr = swapChain_->GetBuffer(1, IID_PPV_ARGS(&swapChainResource_[1]));
	assert(SUCCEEDED(hr));
}


///=====================================================/// 
///RTVの作成
///=====================================================/// 
void DirectXManager::CreateRenderTargetViews() {
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


///=====================================================/// 
///Fenceの生成
///=====================================================/// 
void DirectXManager::FenceGeneration() {
	fenceValue_++;
	//GPUがここまでたどり着いついたときに、Fenceの値を指定した値に代入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), fenceValue_);
	//GetCompketedvalueの初期値はFence作成時に渡した初期値
	if (fence_->GetCompletedValue() < fenceValue_) {
		//指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		//イベントを待つ
		WaitForSingleObject(fenceEvent_, INFINITE);
	}
}


///=====================================================/// 
///コマンド積み込んで確定させる
///=====================================================/// 
void DirectXManager::SettleCommandList() {
	//これから書き込むバックバッファのインデックスを取得
	backBufferIndex_ = swapChain_->GetCurrentBackBufferIndex();
}


///=====================================================/// 
///TransitionBarrierを張る
///=====================================================/// 
void DirectXManager::SetupTransitionBarrier() {
	//TransitionBarrierの設定

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


///=====================================================/// 
///RenderTargetの設定
///=====================================================/// 
void DirectXManager::RenderTargetPreference() {
	//描画先のRTVを設定する
	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex_], false, &dsvHandle_);
	//指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f }; // この色を変更することでウィンドウの色を変更できます
	commandList_->ClearRenderTargetView(rtvHandles_[backBufferIndex_], clearColor, 0, nullptr);
	commandList_->ClearDepthStencilView(dsvHandle_, D3D12_CLEAR_FLAG_DEPTH, 1.0F, 0, 0, nullptr);
}

///=====================================================/// 
///コマンドリストの決定
///=====================================================/// 
void DirectXManager::CloseCommandList() {

	//画面に書く処理はすべて終わり。画面に映すので状態を遷移
	//今回はRenderTargetからPresebtにする
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	//TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier_);
	//コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
	hr = commandList_->Close();
	assert(SUCCEEDED(hr));
}

///=====================================================/// 
///コマンドのキック
///=====================================================/// 
void DirectXManager::ExecuteCommandList() {
	//GPUにコマンドリストの実行を行わせる
	Microsoft::WRL::ComPtr <ID3D12CommandList> commandLists[] = { commandList_ };
	commandQueue_->ExecuteCommandLists(1, commandLists->GetAddressOf());
	//GPUとOSに画面の交換を行うように通知する
	swapChain_->Present(1, 0);

	fenceValue_++;
	//GPUがここまでたどり着いついたときに、Fenceの値を指定した値に代入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), fenceValue_);
	//GetCompketedvalueの初期値はFence作成時に渡した初期値
	if (fence_->GetCompletedValue() < fenceValue_) {
		//指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		//イベントを待つ
		WaitForSingleObject(fenceEvent_, INFINITE);
	}

	//次フレーム用のコマンドリストを準備
	hr = commandAllocator_->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr));
}

///=====================================================/// 
///開放処理
///=====================================================/// 
void DirectXManager::ReleaseResources() {
	CloseHandle(fenceEvent_);
#ifdef _DEBUG
	//debugController_->Release();
#endif // DEBUG
	CloseWindow(winApp_->GetWindowHandle());
}

///=====================================================/// 
///リソースリークチェック
///=====================================================/// 
void DirectXManager::CheckResourceLeaks() {
	Microsoft::WRL::ComPtr <IDXGIDebug> debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
		//開放を忘れてエラーが出た場合、205行目をコメントアウト
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
	}
}






///=====================================================/// 
///生成関数
///=====================================================///

///-------------------------------------------/// 
///深度BufferステンシルBufferの生成関数
///-------------------------------------------///
Microsoft::WRL::ComPtr<ID3D12Resource> DirectXManager::CreateDepthStencilTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height) {
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
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,					//Heapの設定
		D3D12_HEAP_FLAG_NONE,				//heepの特殊な設定。特になし。
		&resourceDesc,						//Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	//深度値を書き込む状態にしておく
		&depthClearValue,					//Clear最適値
		IID_PPV_ARGS(&resource));			//作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));
	return resource;
}

///-------------------------------------------/// 
///DescriptorHeap関数
///-------------------------------------------///
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DirectXManager::CreateDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(hr));
	// 成功したログを出力
	Log("Descriptor heap created successfully.");
	return descriptorHeap;
}


