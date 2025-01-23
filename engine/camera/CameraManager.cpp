/*********************************************************************
 * \file   CameraManager.cpp
 * \brief
 *
 * \author Harukichimaru
 * \date   January 2025
 * \note
 *********************************************************************/
#include "CameraManager.h"
#include "ImguiSetup.h"
#include "Input.h"
#include "MathFunc4x4.h"
#include "AffineTransformations.h"

///=============================================================================
///						シングルトンインスタンスの取得
CameraManager *CameraManager::GetInstance() {
	static CameraManager instance;
	return &instance;
}

///=============================================================================
///						終了処理
void CameraManager::Finalize() {
	cameras_.clear();
}

///=============================================================================
///						初期化
void CameraManager::Initialize() {
	//デバックカメラの追加
	AddCamera("DebugCamera");
	//デフォルトカメラの追加
	AddCamera("DefaultCamera");
	//デフォルトカメラの設定
	SetCurrentCamera("DefaultCamera");
}

///=============================================================================
///                     カメラの追加
void CameraManager::AddCamera(const std::string &name) {
	//カメラを作成
	std::unique_ptr<Camera> camera = std::make_unique<Camera>();
	camera->Initialize();

	//カメラを登録
	cameras_[name] = std::move(camera);
}

///=============================================================================
///						カメラの取得
Camera *CameraManager::GetCamera(const std::string &name) const {
	auto it = cameras_.find(name);
	if(it != cameras_.end()) {
		return it->second.get();
	}
	return nullptr;
}

///=============================================================================
///						使用カメラの設定
void CameraManager::SetCurrentCamera(const std::string &name) {
	if(cameras_.find(name) != cameras_.end()) {
		currentCameraName_ = name;
	}
}

///=============================================================================
///                     現在のカメラの取得
Camera *CameraManager::GetCurrentCamera() const {
	return GetCamera(currentCameraName_);
}

///=============================================================================
///                     カメラの更新
void CameraManager::UpdateAll() {
	for(auto &pair : cameras_) {
		if(pair.second) {
			pair.second->Update();
		}
	}
    // デバックカメラの更新
	DebugCameraUpdate();
}

///=============================================================================
///						デバックカメラの更新
void CameraManager::DebugCameraUpdate() {
    // デバッグカメラを取得
    Camera *debugCamera = GetCamera("DebugCamera");

    // カメラが存在しない場合は処理しない
    if(!debugCamera) {
        return;
    }

    // 入力クラスのインスタンスを取得
    Input *input = Input::GetInstance();

    // マウスの移動量を取得
    float mouseDx, mouseDy;
    mouseDx = input->GetMouseMove().x;
    mouseDy = input->GetMouseMove().y;

    // マウスのホイール量を取得（ズームに使用）
    float mouseWheel = input->GetMouseWheel();

    // マウスのボタン状態を取得
    bool isLeftButtonPressed = input->PushMouseButton(0);
    bool isMiddleButtonPressed = input->PushMouseButton(2);
   // bool isRightButtonPressed = input->PushMouseButton(1);

    // カメラのトランスフォームを取得
    Transform cameraTransform = debugCamera->GetTransform();

    // 中心点を基準にした回転
    static Vector3 targetPoint = {0.0f, 0.0f, 0.0f}; // 回転の中心点
    static float distanceToTarget = 5.0f; // 中心点とカメラの距離

    if(isLeftButtonPressed) {
        // 回転速度の調整
        const float rotateSpeed = 0.005f;

        // マウス移動量に応じてカメラの回転を更新
        cameraTransform.rotate.y -= mouseDx * rotateSpeed;
        cameraTransform.rotate.x -= mouseDy * rotateSpeed;

        // 回転行列を作成
        Matrix4x4 rotationMatrix = MakeRotateMatrix(cameraTransform.rotate);

        // カメラの位置を更新
        cameraTransform.translate = targetPoint - Conversion({ 0.0f, 0.0f, 1.0f }, rotationMatrix) * distanceToTarget;
    }

    // パン（カメラの平行移動）
    if(isMiddleButtonPressed) {
        // パン速度の調整
        const float panSpeed = 0.01f;

        // カメラの向きから右方向と上方向のベクトルを計算
        Matrix4x4 rotationMatrix = MakeRotateMatrix(cameraTransform.rotate);
        Vector3 right = { rotationMatrix.m[0][0], rotationMatrix.m[1][0], rotationMatrix.m[2][0] };
        Vector3 up = { rotationMatrix.m[0][1], rotationMatrix.m[1][1], rotationMatrix.m[2][1] };

        // マウス移動量に応じてカメラの位置を更新
        cameraTransform.translate = cameraTransform.translate - ( right * ( mouseDx * panSpeed ) );
        cameraTransform.translate = cameraTransform.translate + ( up * ( mouseDy * panSpeed ) );
        targetPoint = targetPoint - ( right * ( mouseDx * panSpeed ) );
        targetPoint = targetPoint + ( up * ( mouseDy * panSpeed ) );
    }

    // WASDキー（中心点の平行移動）
    const float moveSpeed = 0.1f;
    Matrix4x4 rotationMatrix = MakeRotateMatrix(cameraTransform.rotate);
    Vector3 forward = { rotationMatrix.m[0][2], rotationMatrix.m[1][2], rotationMatrix.m[2][2] };
    Vector3 right = { rotationMatrix.m[0][0], rotationMatrix.m[1][0], rotationMatrix.m[2][0] };

    if(input->PushKey(DIK_UPARROW)) {
        targetPoint = targetPoint + forward * moveSpeed;
    }
    if(input->PushKey(DIK_DOWNARROW)) {
        targetPoint = targetPoint - forward * moveSpeed;
    }
    if(input->PushKey(DIK_LEFTARROW)) {
        targetPoint = targetPoint - right * moveSpeed;
    }
    if(input->PushKey(DIK_RIGHTARROW)) {
        targetPoint = targetPoint + right * moveSpeed;
    }

    // ズーム（中心点とカメラの距離の変更）
    if(mouseWheel != 0.0f) {
        // ズーム速度の調整
        const float zoomSpeed = 0.01f;

        // ホイールの回転量に応じて距離を更新
        distanceToTarget -= mouseWheel * zoomSpeed;
        if (distanceToTarget < 0.1f) {
            distanceToTarget = 0.1f; // 距離が負になるのを防ぐ
        }

        // カメラの位置を更新
        cameraTransform.translate = targetPoint - Conversion({ 0.0f, 0.0f, 1.0f }, rotationMatrix) * distanceToTarget;
    }

    // 更新されたトランスフォームをカメラに反映
    debugCamera->SetTransform(cameraTransform);
}


///=============================================================================
///						デバックカメラへの切り替え
void CameraManager::ChangeDebugCamera() {
	if(currentCameraName_ != "DebugCamera") {
		previousCameraName_ = currentCameraName_;
		SetCurrentCamera("DebugCamera");
	} else {
		SetCurrentCamera(previousCameraName_);
	}
}

///=============================================================================
///						Imgui描画
void CameraManager::DrawImGui() {
	// カメラの情報を表示
	ImGui::Begin("Camera");
	ImGui::Text("Current Camera: %s", currentCameraName_.c_str());
	ImGui::Separator();
	for(auto &pair : cameras_) {
		if(pair.second) {
			// カメラの情報を表示
			ImGui::Text("Camera Name: %s", pair.first.c_str());
			ImGui::Text("Position: %.2f, %.2f, %.2f", pair.second->GetTransform().translate.x, pair.second->GetTransform().translate.y, pair.second->GetTransform().translate.z);
			ImGui::Text("Rotation: %.2f, %.2f, %.2f", pair.second->GetTransform().rotate.x, pair.second->GetTransform().rotate.y, pair.second->GetTransform().rotate.z);
			ImGui::Text("Scale: %.2f, %.2f, %.2f", pair.second->GetTransform().scale.x, pair.second->GetTransform().scale.y, pair.second->GetTransform().scale.z);
			ImGui::Separator();
		}
	}
	ImGui::End();
}


///=============================================================================
///                     デストラクタ
CameraManager::~CameraManager() {
	cameras_.clear();
}
