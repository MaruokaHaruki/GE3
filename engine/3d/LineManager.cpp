/*********************************************************************
 * \file   LineManager.cpp
 * \brief  
 * 
 * \author Harukichimaru
 * \date   January 2025
 * \note   
 *********************************************************************/
#include "LineManager.h"
#include "ImguiSetup.h"

///=============================================================================
///						インスタンス
LineManager *LineManager::instance_ = nullptr;

///=============================================================================
///						インスタンス設定
LineManager *LineManager::GetInstance() {
	if(instance_ == nullptr) {
		instance_ = new LineManager();
	}
	return instance_;
}

///=============================================================================
///						初期化
void LineManager::Initialize(DirectXCore *dxCore, SrvSetup *srvSetup) {
	//========================================
	// 引数でdxManagerを受取
	dxCore_ = dxCore;
	// 引数でsrvSetupを受取
	srvSetup_ = srvSetup;
	//========================================
	// ラインセットアップの生成
	lineSetup_ = std::make_unique<LineSetup>();
	// ラインセットアップの初期化
	lineSetup_->Initialize(dxCore_, nullptr);
	//========================================
	// ラインの初期化
	line_ = std::make_unique<Line>();
	// ラインの初期化
	line_->Initialize(lineSetup_.get());
}

///=============================================================================
///						終了処理
void LineManager::Finalize() {
	//インスタンスの削除
	delete instance_;
	instance_ = nullptr;
}

///=============================================================================
///						更新処理
void LineManager::Update() {
	// ラインの更新
	line_->Update();
}

///=============================================================================
///						ラインの描画
void LineManager::Draw() {
	//========================================
	// 共通描画設定
	lineSetup_->CommonDrawSetup();
	// ラインの描画
	line_->Draw();

	//========================================
	// ラインのクリア
	line_->ClearLines();
}

///=============================================================================
///						Imguiの描画
void LineManager::DrawImGui() {
	//========================================
	// すべての情報の表示
	ImGui::Begin("LineManager");
	// ラインの位置
	ImGui::Text("Line Position");
	Transform transform = line_->GetTransform();
	// ラインの位置の表示
	ImGui::Text("Position: %f, %f, %f", transform.translate.x, transform.translate.y, transform.translate.z);
	// ラインの回転の表示
	ImGui::Text("Rotation: %f, %f, %f", transform.rotate.x, transform.rotate.y, transform.rotate.z);
	// ラインのスケールの表示
	ImGui::Text("Scale: %f, %f, %f", transform.scale.x, transform.scale.y, transform.scale.z);

	ImGui::End();
}

///=============================================================================
///						ラインのクリア
void LineManager::ClearLines() {
	// ラインのクリア
	line_->ClearLines();
}

///=============================================================================
///						ラインの追加
void LineManager::DrawLine(const Vector3 &start, const Vector3 &end, const Vector4 &color) {
	// ラインの追加
	line_->DrawLine(start, end, color);
}

///=============================================================================
///						グリッドの描画
void LineManager::DrawGrid(const Vector3 &start, const Vector3 &end, const Vector4 &color, int gridNum) {
	// グリッドの幅と奥行きを計算
	float width = end.x - start.x;
	float depth = end.z - start.z;

	// 縦線を描画
	for (int i = 0; i <= gridNum; ++i) {
		float x = start.x + (width / gridNum) * i;
		line_->DrawLine(Vector3(x, start.y, start.z), Vector3(x, start.y, end.z), color);
	}

	// 横線を描画
	for (int i = 0; i <= gridNum; ++i) {
		float z = start.z + (depth / gridNum) * i;
		line_->DrawLine(Vector3(start.x, start.y, z), Vector3(end.x, start.y, z), color);
	}
}
