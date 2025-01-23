#include "Input.h"
#include "ImguiSetup.h"
#include <cmath> // 追加
#include <cassert>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

/// <summary>
/// インスタンスの取得
/// </summary>
/// <returns>Inputのインスタンス</returns>
Input *Input::GetInstance() {
	static Input instance;
	return &instance;
}

///=============================================================================
///						初期化
void Input::Initialize(HINSTANCE hInstance, HWND hwnd) {

	HRESULT result;

	/// ===DirectInputのインスタンス生成=== ///
	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&directInput, nullptr);
	assert(SUCCEEDED(result));

	/// ===マウスデバイス生成=== ///
	result = directInput->CreateDevice(GUID_SysMouse, &mouse_, NULL);
	assert(SUCCEEDED(result));

	// マウスデバイスのデータフォーマットを設定
	result = mouse_->SetDataFormat(&c_dfDIMouse2);
	assert(SUCCEEDED(result));

	// マウスデバイスの協調レベルを設定
	result = mouse_->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	assert(SUCCEEDED(result));

	/// ===キーボードデバイス生成=== ///
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	/// ===入力データ形式のセット=== ///
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));

	/// ===排他抑制レベルのセット=== ///
	result = keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	/// ===コントローラーデバイス生成=== ///
	result = directInput->CreateDevice(GUID_Joystick, &controller, NULL);
	if(SUCCEEDED(result)) {
		/// ===入力データ形式のセット=== ///
		result = controller->SetDataFormat(&c_dfDIJoystick2);
		assert(SUCCEEDED(result));

		/// ===排他抑制レベルのセット=== ///
		result = controller->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		assert(SUCCEEDED(result));

		controllerConnected = true;
	} else {
		controllerConnected = false;
	}
}

///=============================================================================
///						更新
void Input::Update() {
	//========================================
	// 前回のマウスの状態を保存
	mouseStatePrev_ = mouseState_;
	HRESULT result = mouse_->Acquire();
	if (SUCCEEDED(result)) {
		result = mouse_->GetDeviceState(sizeof(DIMOUSESTATE2), &mouseState_);
		if (FAILED(result)) {
			ZeroMemory(&mouseState_, sizeof(mouseState_));
		}
	}

	//========================================
	// 前回のキーボードの状態を保存
	memcpy(keyPre, key, sizeof(key));
	//キーボード情報の取得開始
	keyboard->Acquire();
	//キーボードの状態を取得
	keyboard->GetDeviceState(sizeof(key), key);

	//========================================
	// コントローラーが接続されている場合
	if(controllerConnected) {
		// 前回のコントローラーの状態を保存
		controllerStatePre = controllerState;
		// コントローラー情報の取得開始
		controller->Acquire();
		// コントローラーの状態を取得
		controller->GetDeviceState(sizeof(DIJOYSTATE2), &controllerState);
	}

}

///=============================================================================
///						マウスの移動量を取得
Vector2 Input::GetMouseMove() const {
	return Vector2(static_cast<float>( mouseState_.lX ), static_cast<float>( mouseState_.lY ));
}

///=============================================================================
///						マウスのホイールの移動量を取得
float Input::GetMouseWheel() const {
	return static_cast<float>( mouseState_.lZ );
}

///=============================================================================
///						マウスのボタンの押下をチェック
bool Input::PushMouseButton(int buttonNumber) const {
	if(mouseState_.rgbButtons[buttonNumber] & 0x80) {
		return true;
	}
	return false;
}

///=============================================================================
///						マウスのボタンが押されているかをチェック
bool Input::TriggerMouseButton(int buttonNumber) const {
	if(( mouseState_.rgbButtons[buttonNumber] & 0x80 ) &&
		!( mouseStatePrev_.rgbButtons[buttonNumber] & 0x80 )) {
		return true;
	}
	return false;
}

///=====================================================/// 
///キーの押下をチェック
///=====================================================///
bool Input::PushKey(BYTE keyNumber) {
	/// ===指定キーを押していればtrueを返す=== ///
	if(key[keyNumber]) {
		return true;
	}

	/// ===そうでなければfalse=== ///
	return false;
}

bool Input::TriggerKey(BYTE keyNumber) {
	/// ===前回は押しておらず、今回は押している場合trueを返す=== ///
	if(key[keyNumber] && !keyPre[keyNumber]) {
		return true;
	}

	return false;
}

///=====================================================/// 
///コントローラーボタンの押下をチェック
///=====================================================///
bool Input::PushButton(BYTE buttonNumber) {
	if(!controllerConnected) {
		return false;
	}

	/// ===指定ボタンを押していればtrueを返す=== ///
	if(controllerState.rgbButtons[buttonNumber]) {
		return true;
	}

	/// ===そうでなければfalse=== ///
	return false;
}

bool Input::TriggerButton(BYTE buttonNumber) {
	if(!controllerConnected) {
		return false;
	}

	/// ===前回は押しておらず、今回は押している場合trueを返す=== ///
	if(controllerState.rgbButtons[buttonNumber] && !controllerStatePre.rgbButtons[buttonNumber]) {
		return true;
	}

	return false;
}
///=============================================================================
/// 左スティックのX軸の値を取得
float Input::GetLeftStickX() const {
	if(!controllerConnected) {
		return 0.0f;
	}
	float rawValue = ApplyDeadZone(controllerState.lX);
	float controllerVal = NormalizeStickValue(rawValue);

	// デッドゾーンを適用
	if(controllerVal >= deadZone || controllerVal <= -deadZone) {
		return controllerVal;
	} else {
		return 0.0f;
	}
}

///=============================================================================
/// 左スティックのY軸の値を取得
float Input::GetLeftStickY() const {
	if(!controllerConnected) {
		return 0.0f;
	}
	float rawValue = ApplyDeadZone(controllerState.lY);
	float controllerVal = NormalizeStickValue(rawValue); // 符号を反転

	// デッドゾーンを適用
	if(controllerVal >= deadZone || controllerVal <= -deadZone) {
		return -controllerVal;
	} else {
		return 0.0f;
	}
}

///=============================================================================
/// 右スティックのX軸の値を取得
float Input::GetRightStickX() const {
	if(!controllerConnected) {
		return 0.0f;
	}
	float rawValue = ApplyDeadZone(controllerState.lRx);
	float controllerVal = NormalizeStickValue(rawValue);

	// デッドゾーンを適用
	if(controllerVal >= deadZone || controllerVal <= -deadZone) {
		return controllerVal;
	} else {
		return 0.0f;
	}
}

///=============================================================================
/// 右スティックのY軸の値を取得
float Input::GetRightStickY() const {
	if(!controllerConnected) {
		return 0.0f;
	}
	float rawValue = ApplyDeadZone(controllerState.lRy);
	float controllerVal = -NormalizeStickValue(rawValue); // 符号を反転

	// デッドゾーンを適用
	if(controllerVal >= deadZone || controllerVal <= -deadZone) {
		return controllerVal;
	} else {
		return 0.0f;
	}
}


///=====================================================/// 
///スティックのデッドゾーンを適用
///=====================================================///
float Input::ApplyDeadZone(LONG value) const {
	float normalizedValue = static_cast<float>( value ) / 32767.0f;
	if(fabs(normalizedValue) < deadZone) {
		return 0.0f;
	}
	if(normalizedValue > 0) {
		return ( normalizedValue - deadZone ) / ( 1.0f - deadZone );
	} else {
		return ( normalizedValue + deadZone ) / ( 1.0f - deadZone );
	}
}

///=============================================================================
///						スティックの値を -1.0 〜 1.0 に正規化する関数
float Input::NormalizeStickValue(float value) const {
	return value - 1.0f;
}

///=====================================================/// 
///デッドゾーンの値を設定
///=====================================================///
//void Input::SetDeadZone(float deadZone) {
//	deadZone = deadZone;
//}

///=====================================================/// 
///左スティックが左に傾いているかをチェック
///=====================================================///
bool Input::IsLeftStickLeft() const {
	return GetLeftStickX() < 0.0f;
}

///=====================================================/// 
///左スティックが右に傾いているかをチェック
///=====================================================///
bool Input::IsLeftStickRight() const {
	return GetLeftStickX() > 0.0f;
}

///=====================================================/// 
///左スティックが上に傾いているかをチェック
///=====================================================///
bool Input::IsLeftStickUp() const {
	return GetLeftStickY() > 0.0f;
}

///=====================================================/// 
///左スティックが下に傊いているかをチェック
///=====================================================///
bool Input::IsLeftStickDown() const {
	return GetLeftStickY() < 0.0f;
}

///=====================================================/// 
///右スティックが左に傊いているかをチェック
///=====================================================///
bool Input::IsRightStickLeft() const {
	return GetRightStickX() < 0.0f;
}

///=====================================================/// 
///右スティックが右に傊いているかをチェック
///=====================================================///
bool Input::IsRightStickRight() const {
	return GetRightStickX() > 0.0f;
}

///=====================================================/// 
///右スティックが上に傊いているかをチェック
///=====================================================///
bool Input::IsRightStickUp() const {
	return GetRightStickY() > 0.0f;
}

///=====================================================/// 
///右スティックが下に傊いているかをチェック
///=====================================================///
bool Input::IsRightStickDown() const {
	return GetRightStickY() < 0.0f;
}

///=============================================================================
///						ImGui描画
void Input::ImGuiDraw() {
	ImGui::Begin("Input");
	//========================================
	// すべてのキーの状態
	ImGui::Text("Keys:");
	for(int i = 0; i < 256; ++i) {
		if(key[i]) {
			ImGui::Text("Key: %d", i);
		}
	}
	//========================================
	// すべてのコントローラの状態
	if(controllerConnected) {
		ImGui::Separator();
		ImGui::Text("Controller:");
		ImGui::Text("Controller Connected");
		ImGui::Text("Left Stick X: %f", GetLeftStickX());
		ImGui::Text("Left Stick Y: %f", GetLeftStickY());
		ImGui::Text("Right Stick X: %f", GetRightStickX());
		ImGui::Text("Right Stick Y: %f", GetRightStickY());
	}
	//========================================
	// マウスの状態
	ImGui::Separator();
	ImGui::Text("Mouse:");
	ImGui::Text("Mouse X: %d", GetMouseMove().x);
	ImGui::Text("Mouse Y: %d", GetMouseMove().y);
	ImGui::Text("Mouse Z: %d", mouseState_.lZ);
	// マウスのボタンの状態
	for(int i = 0; i < 8; ++i) {
		if(mouseState_.rgbButtons[i]) {
			ImGui::Text("Mouse Button: %d", i);
		}
	}

	ImGui::End();

}
