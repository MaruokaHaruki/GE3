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
//========================================
// 数学関数のインクルード
#define _USE_MATH_DEFINES
#include <math.h>

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
	//Gridの描画
	DrawGrid(gridSize_, gridDivisions_, gridColor_);
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
	// 描画設定
	ImGui::Begin("LineManager");
	//========================================
	// Lineを描画するか
	ImGui::Checkbox("Line", &isDrawLine_);
	ImGui::Separator();
	//========================================
	// Gridの描画
	ImGui::Checkbox("Grid", &isDrawGrid_);
	//Gridの設定
	ImGui::SliderFloat("GridSize", &gridSize_, 1.0f, 10000.0f);
	ImGui::SliderInt("Divisions", &gridDivisions_, 1, 512);
	//色
	ImGui::ColorEdit4("Color", &gridColor_.x);
	//セパレーター
	ImGui::Separator();
	//========================================
	// Sphereの描画
	ImGui::Checkbox("Sphere", &isDrawSphere_);
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
	if(!isDrawLine_) {
		return;
	}
#ifdef _DEBUG
	// ラインの追加
	line_->DrawLine(start, end, color);
#endif // _DEBUG
}

///=============================================================================
///						グリッドの描画
void LineManager::DrawGrid(float gridSize, int divisions, const Vector4 &color) {
	if(!isDrawGrid_ || divisions <= 0) {
		return;
	}
	float halfSize = gridSize * 0.5f;
	float step = gridSize / divisions;

	for(int i = 0; i <= divisions; ++i) {
		float offset = -halfSize + ( i * step );

		// X軸に平行な線
		DrawLine(Vector3(-halfSize, 0.0f, offset), Vector3(halfSize, 0.0f, offset), color);

		// Z軸に平行な線
		DrawLine(Vector3(offset, 0.0f, -halfSize), Vector3(offset, 0.0f, halfSize), color);
	}
}

///=============================================================================
///						球体の描画
void LineManager::DrawSphere(const Vector3 &center, float radius, const Vector4 &color, int divisions) {
	if(!isDrawSphere_ || divisions <= 0) {
		return;
	}
	float angleStep = 2.0f * static_cast<float>( M_PI ) / divisions;

	// XY, XZ, YZ 平面の円を描画
	for(int i = 0; i < divisions; ++i) {
		float angle1 = angleStep * i;
		float angle2 = angleStep * ( i + 1 );

		// XY 平面の円
		DrawLine(
			Vector3(center.x + radius * cosf(angle1), center.y + radius * sinf(angle1), center.z),
			Vector3(center.x + radius * cosf(angle2), center.y + radius * sinf(angle2), center.z),
			color
		);

		// XZ 平面の円
		DrawLine(
			Vector3(center.x + radius * cosf(angle1), center.y, center.z + radius * sinf(angle1)),
			Vector3(center.x + radius * cosf(angle2), center.y, center.z + radius * sinf(angle2)),
			color
		);

		// YZ 平面の円
		DrawLine(
			Vector3(center.x, center.y + radius * cosf(angle1), center.z + radius * sinf(angle1)),
			Vector3(center.x, center.y + radius * cosf(angle2), center.z + radius * sinf(angle2)),
			color
		);
	}

	// 緯度方向の分割を追加
	for(int lat = 1; lat < divisions / 2; ++lat) {
		float latAngle1 = static_cast<float>( M_PI ) * lat / ( divisions / 2 );
		float latAngle2 = static_cast<float>( M_PI ) * ( lat + 1 ) / ( divisions / 2 );

		float r1 = radius * sinf(latAngle1);
		float r2 = radius * sinf(latAngle2);
		float y1 = center.y + radius * cosf(latAngle1);
		float y2 = center.y + radius * cosf(latAngle2);

		for(int i = 0; i < divisions; ++i) {
			float angle1 = angleStep * i;
			float angle2 = angleStep * ( i + 1 );

			// 緯度方向の円
			DrawLine(
				Vector3(center.x + r1 * cosf(angle1), y1, center.z + r1 * sinf(angle1)),
				Vector3(center.x + r1 * cosf(angle2), y1, center.z + r1 * sinf(angle2)),
				color
			);

			DrawLine(
				Vector3(center.x + r2 * cosf(angle1), y2, center.z + r2 * sinf(angle1)),
				Vector3(center.x + r2 * cosf(angle2), y2, center.z + r2 * sinf(angle2)),
				color
			);
		}
	}

	// 経度方向の線を追加
	for(int lon = 0; lon < divisions; ++lon) {
		float lonAngle = angleStep * lon;
		float nextLonAngle = angleStep * ( lon + 1 );

		for(int lat = 0; lat <= divisions / 2; ++lat) {
			float latAngle = static_cast<float>( M_PI ) * lat / ( divisions / 2 );
			float nextLatAngle = static_cast<float>( M_PI ) * ( lat + 1 ) / ( divisions / 2 );

			float r1 = radius * sinf(latAngle);
			float r2 = radius * sinf(nextLatAngle);
			float y1 = center.y + radius * cosf(latAngle);
			float y2 = center.y + radius * cosf(nextLatAngle);

			DrawLine(
				Vector3(center.x + r1 * cosf(lonAngle), y1, center.z + r1 * sinf(lonAngle)),
				Vector3(center.x + r2 * cosf(lonAngle), y2, center.z + r2 * sinf(lonAngle)),
				color
			);
		}
	}
}




