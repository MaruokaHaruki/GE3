/*********************************************************************
 * \file   ImguiSetup.cpp
 * \brief
 *
 * \author Harukichimaru
 * \date   December 2024
 * \note
 *********************************************************************/
#include "ImguiSetup.h"
#include <windows.h>
#include <psapi.h>
//---------------------------------------
// ImGui
#include "imgui.h"
#include "imgui_internal.h"
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

///=============================================================================
///						初期化
void ImguiSetup::Initialize(WinApp* winApp, DirectXCore* dxCore, Style style) {
	//========================================
	// ポインタの受取
	winApp_ = winApp; // ウィンドウズアプリケーションクラス
	dxCore_ = dxCore; // DirectXコアクラス

	//========================================
	// ImGuiの設定
	//---------------------------------------
	// ImGuiのコンテキストを生成
	// NOTE:複数枚作ってフォントを変えることもできる
	ImGui::CreateContext();
	//---------------------------------------
	// ImGUiのスタイルを設定
	if(Style::CLASSIC == style) {
		ImGui::StyleColorsClassic();
	} else if(Style::LIGHT == style) {
		ImGui::StyleColorsLight();
	} else if(Style::CYBER == style) {
		StyleColorsCyberGreen(ImGui::GetStyle());
	} else if(Style::GREEN == style) {
		StyleColorsDarkGreen(ImGui::GetStyle());
	} else {
		ImGui::StyleColorsDark();
	}

	//---------------------------------------
	



	//========================================
	// Win32用の初期化
	ImGui_ImplWin32_Init(winApp_->GetWindowHandle());

	//========================================
	// デスクリプタヒープの設定
	// NOTE:ImGuiの描画に必要なSRV用ディスクリプタヒープを生成
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = 1;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	// ディスクリプタヒープの生成
	dxCore->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&srvDescriptorHeap_));
	//生成確認
	assert(srvDescriptorHeap_);


	//========================================
	// DirectX12用の初期化
	ImGui_ImplDX12_Init(dxCore_->GetDevice().Get(),
		dxCore_->GetSwapChainDesc().BufferCount,
		dxCore_->GetRtvDesc().Format,
		srvDescriptorHeap_.Get(),
		srvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap_->GetGPUDescriptorHandleForHeapStart()
	);
}

///=============================================================================
///						ImGuiの受付開始処理
void ImguiSetup::Begin() {
	//========================================
	// ImGuiのフレーム開始
	ImGui_ImplDX12_NewFrame();	// ImGuiのDirectX12サポート開始
	ImGui_ImplWin32_NewFrame(); // ImGuiのWin32サポート開始
	ImGui::NewFrame();			// ImGuiのフレーム開始

	//========================================
	// 
	ShowPerformanceMonitor();
}

///=============================================================================
///						ImGuiの受付終了処理
void ImguiSetup::End() {
	//========================================
	// ImGuiのフレーム終了
	ImGui::Render();			// ImGuiのレンダリング

}

///=============================================================================
///						ImGuiの描画
void ImguiSetup::Draw() {
	//========================================
	// GPUコマンドの発行
	ID3D12GraphicsCommandList* commandList = dxCore_->GetCommandList().Get();

	//デスクリプタヒープの配列をセットするコマンド
	ID3D12DescriptorHeap* ppHeaps[] = { srvDescriptorHeap_.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//描画コマンド
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
}

///=============================================================================
///						終了処理
void ImguiSetup::Finalize() {
	//========================================
	// ImGuiの終了処理
	ImGui_ImplDX12_Shutdown();		// ImGuiのDirectX12サポート終了
	ImGui_ImplWin32_Shutdown();		// ImGuiのWin32サポート終了
	ImGui::DestroyContext();		// ImGuiコンテキストの破棄

	//========================================
	// ディスクリプタヒープの解放
	srvDescriptorHeap_.Reset();
}

///=============================================================================
///						ImGuiのサイバースタイル
void ImguiSetup::StyleColorsCyberGreen(ImGuiStyle& style) {
	//========================================
	// スタイルの設定
	style.WindowRounding = 5.0f; // ウィンドウの角を丸くする
	style.FrameRounding = 4.0f;  // フレームの角を丸くする

    // カスタムスタイルの設定
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text]				= ImVec4(0.0f, 0.9f, 0.0f, 0.5f);      // テキスト色
    colors[ImGuiCol_WindowBg]			= ImVec4(0.0f, 0.0f, 0.0f, 0.4f);      // ウィンドウ背景（透過）
    colors[ImGuiCol_Border]				= ImVec4(0.0f, 0.9f, 0.0f, 0.4f);      // 枠線
    colors[ImGuiCol_FrameBg]			= ImVec4(0.0f, 0.2f, 0.0f, 0.4f);      // フレーム背景
    colors[ImGuiCol_FrameBgHovered]		= ImVec4(0.0f, 0.7f, 0.0f, 0.4f);      // フレーム背景（ホバー時）
    colors[ImGuiCol_FrameBgActive]		= ImVec4(0.0f, 0.9f, 0.0f, 0.4f);      // フレーム背景（アクティブ時）
    colors[ImGuiCol_TitleBg]			= ImVec4(0.0f, 0.4f, 0.0f, 0.4f);      // タイトル背景
    colors[ImGuiCol_TitleBgActive]		= ImVec4(0.0f, 0.6f, 0.0f, 0.4f);      // タイトル背景（アクティブ時）
    colors[ImGuiCol_CheckMark]			= ImVec4(0.0f, 0.9f, 0.0f, 1.0f);      // チェックマーク
    colors[ImGuiCol_SliderGrab]			= ImVec4(0.0f, 0.9f, 0.0f, 1.0f);      // スライダー
    colors[ImGuiCol_Button]				= ImVec4(0.0f, 0.4f, 0.0f, 0.4f);      // ボタン
    colors[ImGuiCol_ButtonHovered]		= ImVec4(0.0f, 0.7f, 0.0f, 0.4f);      // ボタン（ホバー時）
    colors[ImGuiCol_ButtonActive]		= ImVec4(0.0f, 0.9f, 0.0f, 0.4f);      // ボタン（アクティブ時）
    colors[ImGuiCol_Header]				= ImVec4(0.0f, 0.4f, 0.0f, 0.4f);      // ヘッダー
    colors[ImGuiCol_HeaderHovered]		= ImVec4(0.0f, 0.7f, 0.0f, 0.4f);      // ヘッダー（ホバー時）
    colors[ImGuiCol_HeaderActive]		= ImVec4(0.0f, 0.9f, 0.0f, 0.4f);      // ヘッダー（アクティブ時）
    colors[ImGuiCol_Separator]			= ImVec4(0.0f, 0.9f, 0.0f, 0.4f);      // セパレーター
    colors[ImGuiCol_ResizeGrip]			= ImVec4(0.0f, 0.4f, 0.0f, 0.4f);      // リサイズグリップ
    colors[ImGuiCol_ResizeGripHovered]	= ImVec4(0.0f, 0.7f, 0.0f, 0.4f);      // リサイズグリップ（ホバー時）
    colors[ImGuiCol_ResizeGripActive]	= ImVec4(0.0f, 0.9f, 0.0f, 0.4f);      // リサイズグリップ（アクティブ時）
    colors[ImGuiCol_Tab]				= ImVec4(0.0f, 0.4f, 0.0f, 0.4f);      // タブ
    colors[ImGuiCol_TabHovered]			= ImVec4(0.0f, 0.7f, 0.0f, 0.4f);      // タブ（ホバー時）
    colors[ImGuiCol_TabActive]			= ImVec4(0.0f, 0.9f, 0.0f, 0.4f);      // タブ（アクティブ時）
    colors[ImGuiCol_PopupBg]			= ImVec4(0.0f, 0.0f, 0.0f, 1.0f);      // ポップアップ背景（透過）
}

///=============================================================================
///						
void ImguiSetup::StyleColorsDarkGreen(ImGuiStyle& style) {
	//========================================
	// スタイルの設定
	style.WindowRounding = 5.0f; // ウィンドウの角を丸くする
	style.FrameRounding = 4.0f;  // フレームの角を丸くする

	ImVec4* colors = style.Colors;
	colors[ImGuiCol_Text] = ImVec4(0.5f, 0.9f, 0.5f, 1.0f);      // テキスト色
	colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.5f);      // ウィンドウ背景（透過）
	colors[ImGuiCol_Border] = ImVec4(0.5f, 0.9f, 0.5f, 0.5f);      // 枠線
	colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.4f, 0.2f, 0.5f);      // フレーム背景
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3f, 0.6f, 0.3f, 0.5f);      // フレーム背景（ホバー時）
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.4f, 0.7f, 0.4f, 0.5f);      // フレーム背景（アクティブ時）
	colors[ImGuiCol_TitleBg] = ImVec4(0.2f, 0.4f, 0.2f, 0.5f);      // タイトル背景
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.3f, 0.6f, 0.3f, 0.5f);      // タイトル背景（アクティブ時）
	colors[ImGuiCol_CheckMark] = ImVec4(0.5f, 0.9f, 0.5f, 1.0f);      // チェックマーク
	colors[ImGuiCol_SliderGrab] = ImVec4(0.5f, 0.9f, 0.5f, 1.0f);      // スライダー
	colors[ImGuiCol_Button] = ImVec4(0.2f, 0.4f, 0.2f, 0.5f);      // ボタン
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.6f, 0.3f, 0.5f);      // ボタン（ホバー時）
	colors[ImGuiCol_ButtonActive] = ImVec4(0.4f, 0.7f, 0.4f, 0.5f);      // ボタン（アクティブ時）
	colors[ImGuiCol_Header] = ImVec4(0.2f, 0.4f, 0.2f, 0.5f);      // ヘッダー
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.3f, 0.6f, 0.3f, 0.5f);      // ヘッダー（ホバー時）
	colors[ImGuiCol_HeaderActive] = ImVec4(0.4f, 0.7f, 0.4f, 0.5f);      // ヘッダー（アクティブ時）
	colors[ImGuiCol_Separator] = ImVec4(0.5f, 0.9f, 0.5f, 0.5f);      // セパレーター
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.2f, 0.4f, 0.2f, 0.5f);      // リサイズグリップ
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.3f, 0.6f, 0.3f, 0.5f);      // リサイズグリップ（ホバー時）
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.4f, 0.7f, 0.4f, 0.5f);      // リサイズグリップ（アクティブ時）
	colors[ImGuiCol_Tab] = ImVec4(0.2f, 0.4f, 0.2f, 0.5f);      // タブ
	colors[ImGuiCol_TabHovered] = ImVec4(0.3f, 0.6f, 0.3f, 0.5f);      // タブ（ホバー時）
	colors[ImGuiCol_TabActive] = ImVec4(0.4f, 0.7f, 0.4f, 0.5f);      // タブ（アクティブ時）
	colors[ImGuiCol_PopupBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.5f);      // ポップアップ背景（透過）
}

void ImguiSetup::ShowPerformanceMonitor() {
	// ImGuiウィンドウを作成
	if (ImGui::Begin("Performance Monitor")) {
		// FPSの表示
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

		// フレームタイムの表示
		float frameTime = 1000.0f / ImGui::GetIO().Framerate;
		ImGui::Text("Frame Time: %.3f ms", frameTime);

		// メモリ使用量（プロセス単位）
		PROCESS_MEMORY_COUNTERS_EX pmc;
		if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
			float memoryUsageMB = pmc.WorkingSetSize / 1024.0f / 1024.0f;
			ImGui::Text("Memory Usage: %.2f MB", memoryUsageMB);
		}

		// 描画コール数の表示
		//ImGui::Text("Draw Calls: %d", drawCallCount);

		// ポリゴン（トライアングル）数の表示
		//ImGui::Text("Triangles: %d", triangleCount);

		// ロジック vs レンダリング処理時間
		//ImGui::Text("Logic Time: %.2f ms", logicTime);
		//ImGui::Text("Rendering Time: %.2f ms", renderingTime);

		// グラフの表示 (FPSの変動)
		static float frameTimes[100] = { 0.0f };
		static int frameIndex = 0;
		frameTimes[frameIndex] = frameTime;
		frameIndex = (frameIndex + 1) % IM_ARRAYSIZE(frameTimes);
		//
		ImGui::PlotLines("Frame Times", frameTimes, IM_ARRAYSIZE(frameTimes), 0, nullptr, 0.0f, 33.0f, ImVec2(0, 80));

		// CPU使用率の取得と表示（Windows限定の簡易例）
		FILETIME idleTime, kernelTime, userTime;
		if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
			static FILETIME prevIdleTime = idleTime, prevKernelTime = kernelTime, prevUserTime = userTime;

			ULARGE_INTEGER idle, kernel, user, prevIdle, prevKernel, prevUser;
			idle.QuadPart = (idleTime.dwLowDateTime | (static_cast<ULONGLONG>(idleTime.dwHighDateTime) << 32));
			kernel.QuadPart = (kernelTime.dwLowDateTime | (static_cast<ULONGLONG>(kernelTime.dwHighDateTime) << 32));
			user.QuadPart = (userTime.dwLowDateTime | (static_cast<ULONGLONG>(userTime.dwHighDateTime) << 32));
			prevIdle.QuadPart = (prevIdleTime.dwLowDateTime | (static_cast<ULONGLONG>(prevIdleTime.dwHighDateTime) << 32));
			prevKernel.QuadPart = (prevKernelTime.dwLowDateTime | (static_cast<ULONGLONG>(prevKernelTime.dwHighDateTime) << 32));
			prevUser.QuadPart = (prevUserTime.dwLowDateTime | (static_cast<ULONGLONG>(prevUserTime.dwHighDateTime) << 32));

			ULONGLONG idleDiff = idle.QuadPart - prevIdle.QuadPart;
			ULONGLONG kernelDiff = kernel.QuadPart - prevKernel.QuadPart;
			ULONGLONG userDiff = user.QuadPart - prevUser.QuadPart;
			ULONGLONG totalDiff = kernelDiff + userDiff;

			float cpuUsage = (totalDiff > 0) ? (1.0f - static_cast<float>(idleDiff) / totalDiff) * 100.0f : 0.0f;

			ImGui::Text("CPU Usage: %.1f%%", cpuUsage);

			prevIdleTime = idleTime;
			prevKernelTime = kernelTime;
			prevUserTime = userTime;
		}

		// 終了
	}
	ImGui::End(); 
}


// NOTE:1.スケールや配置に関する設定
//プロパティ			型		説明
//Alpha	float		UI		全体の透明度を設定 (0.0～1.0)。
//DisabledAlpha		float	非アクティブな要素の透明度 (0.0～1.0)。
//WindowPadding		ImVec2	ウィンドウの内側のパディング (幅, 高さ)。
//WindowRounding	float	ウィンドウの角の丸みの半径。
//WindowBorderSize	float	ウィンドウの枠線の太さ。
//WindowMinSize		ImVec2	ウィンドウの最小サイズ。
//WindowTitleAlign	ImVec2	ウィンドウタイトルの水平・垂直位置 (0.0 左寄せ, 1.0 右寄せ)。
//ChildRounding		float	子ウィンドウの角の丸みの半径。
//ChildBorderSize	float	子ウィンドウの枠線の太さ。
//PopupRounding		float	ポップアップウィンドウの角の丸みの半径。
//PopupBorderSize	float	ポップアップウィンドウの枠線の太さ。
//FramePadding		ImVec2	ボタンやテキスト入力の内側のパディング (幅, 高さ)。
//FrameRounding		float	ボタンやテキスト入力の角の丸みの半径。
//FrameBorderSize	float	ボタンやテキスト入力の枠線の太さ。
//ItemSpacing		ImVec2	隣り合う要素間のスペース。
//ItemInnerSpacing	ImVec2	内部要素間のスペース (例: テキストとスピンボックス間)。
//IndentSpacing		float	ツリーやリストでのインデント幅。
//ScrollbarSize		float	スクロールバーの太さ。
//ScrollbarRounding	float	スクロールバーの角の丸みの半径。
//GrabMinSize		float	スライダーのハンドルの最小サイズ。
//GrabRounding		float	スライダーのハンドルの角の丸みの半径。