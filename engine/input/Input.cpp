#include "Input.h"
#include "ImguiSetup.h"
#include <windows.h>
#include <cassert>
#include <cmath>
#include <algorithm>
#include <dinput.h>

#pragma comment(lib, "xinput.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

///=============================================================================
///						インスタンスの取得
Input *Input::GetInstance() {
	static Input instance;
	return &instance;
}

///=============================================================================
///						デストラクタ
Input::~Input() {
	if(keyboardDevice_) {
		keyboardDevice_->Unacquire();
		keyboardDevice_->Release();
		keyboardDevice_ = nullptr;
	}
	if(directInput_) {
		directInput_->Release();
		directInput_ = nullptr;
	}
}

///=============================================================================
///						初期化
void Input::Initialize(HINSTANCE hInstance, HWND hwnd) {
	// ウィンドウハンドルとインスタンスハンドルを保持
	hwnd_ = hwnd;
	hInstance_ = hInstance;
	//========================================
	// DirectInputの初期化
	HRESULT hr = DirectInput8Create(hInstance_, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&directInput_, NULL);
	assert(SUCCEEDED(hr) && "Failed to create DirectInput8");
	//========================================
	// キーボードデバイスの作成
	hr = directInput_->CreateDevice(GUID_SysKeyboard, &keyboardDevice_, NULL);
	assert(SUCCEEDED(hr) && "Failed to create keyboard device");
	//========================================
	// データフォーマットのセット
	hr = keyboardDevice_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr) && "Failed to set data format for keyboard");
	//========================================
	// 協調レベルの設定
	hr = keyboardDevice_->SetCooperativeLevel(hwnd_, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	assert(SUCCEEDED(hr) && "Failed to set cooperative level for keyboard");
	//========================================
	// デバイスの取得開始
	hr = keyboardDevice_->Acquire();
	assert(SUCCEEDED(hr) && "Failed to acquire keyboard device");
	//========================================
	// マウスの初期位置を取得
	GetCursorPos(&mousePosPrev_);
	ScreenToClient(hwnd_, &mousePosPrev_);
	//========================================
	// キーボードの初期状態を取得
	memcpy(keyStatePrev_, keyState_, sizeof(keyState_));
	//========================================
	// コントローラーの初期状態を取得
	ZeroMemory(&controllerStatePrev_, sizeof(XINPUT_STATE));
	DWORD result = XInputGetState(0, &controllerStatePrev_);
	controllerConnected_ = ( result == ERROR_SUCCESS );
}

///=============================================================================
///						更新
void Input::Update() {
	//========================================
	// マウスの状態を更新
	mousePosPrev_ = mousePos_;
	GetCursorPos(&mousePos_);
	ScreenToClient(hwnd_, &mousePos_);
	//========================================
	// マウスホイールの前回値を保存
	mouseWheelPrev_ = mouseWheel_;
	//========================================
	// マウスボタンの仮想キーコードを配列で定義
	const int mouseVKCodes[3] = { VK_LBUTTON, VK_RBUTTON, VK_MBUTTON };
	//========================================
	// マウスボタンの状態を更新
	for(int i = 0; i < 3; ++i) {
		mouseButtonsPrev_[i] = mouseButtons_[i];
		mouseButtons_[i] = ( GetAsyncKeyState(mouseVKCodes[i]) & 0x8000 ) != 0;
	}
	// マウスホイールの値を更新後、リセット
	mouseWheel_ = 0.0f;
	//========================================
	// キーボードの状態を更新
	memcpy(keyStatePrev_, keyState_, sizeof(keyState_));
	HRESULT hr = keyboardDevice_->GetDeviceState(sizeof(keyState_), keyState_);
	if(FAILED(hr)) {
		// デバイスがロストしている場合
		while(keyboardDevice_->Acquire() == DIERR_INPUTLOST) {}
		keyboardDevice_->GetDeviceState(sizeof(keyState_), keyState_);
	}
	//========================================
	// コントローラーの状態を更新
	controllerStatePrev_ = controllerState_;
	ZeroMemory(&controllerState_, sizeof(XINPUT_STATE));
	DWORD result = XInputGetState(0, &controllerState_);
	controllerConnected_ = ( result == ERROR_SUCCESS );
}

///=============================================================================
///						マウスホイールの値を更新
void Input::OnMouseWheel(short delta) {
	mouseWheel_ += static_cast<float>( delta ) / WHEEL_DELTA;
}

///=============================================================================
///						マウスの移動量を取得
Vector2 Input::GetMouseMove() const {
	return Vector2(
		static_cast<float>( mousePos_.x - mousePosPrev_.x ),
		static_cast<float>( mousePos_.y - mousePosPrev_.y )
	);
}

///=============================================================================
///						ウィンドウの中心からのマウスの位置を取得
Vector2 Input::GetMousePosFromWindowCenter() const {
	RECT rect;
	GetClientRect(hwnd_, &rect);
	float centerX = ( rect.right - rect.left ) / 2.0f;
	float centerY = ( rect.bottom - rect.top ) / 2.0f;

	return Vector2(
		static_cast<float>( mousePos_.x ) - centerX,
		static_cast<float>( mousePos_.y ) - centerY
	);
}

///=============================================================================
///						マウスホイールの移動量を取得
float Input::GetMouseWheel() const {
	return mouseWheel_ * 16.0f;
}

///=============================================================================
///						マウスのボタンの押下をチェック
bool Input::PushMouseButton(int buttonNumber) const {
	if(buttonNumber < 0 || buttonNumber >= 3) {
		return false;
	}
	return mouseButtons_[buttonNumber];
}

///=============================================================================
///						マウスのボタンのトリガーチェック
bool Input::TriggerMouseButton(int buttonNumber) const {
	if(buttonNumber < 0 || buttonNumber >= 3) {
		return false;
	}
	return mouseButtons_[buttonNumber] && !mouseButtonsPrev_[buttonNumber];
}

///=============================================================================
///						キーボード
///--------------------------------------------------------------
///						 キーの押下をチェック
bool Input::PushKey(int keyCode) const {
	return ( keyState_[keyCode] & 0x80 ) != 0;
}

///--------------------------------------------------------------
///						 キーのトリガーチェック
bool Input::TriggerKey(int keyCode) const {
	return ( ( keyState_[keyCode] & 0x80 ) != 0 ) && ( ( keyStatePrev_[keyCode] & 0x80 ) == 0 );
}

///=============================================================================
///						コントローラ
///--------------------------------------------------------------
///						 ボタンの押下をチェック
bool Input::PushButton(WORD button) const {
	if(!controllerConnected_) {
		return false;
	}
	return ( controllerState_.Gamepad.wButtons & button ) != 0;
}

///--------------------------------------------------------------
///						 ボタンのトリガーチェック
bool Input::TriggerButton(WORD button) const {
	if(!controllerConnected_) {
		return false;
	}
	return ( ( controllerState_.Gamepad.wButtons & button ) != 0 ) && ( ( controllerStatePrev_.Gamepad.wButtons & button ) == 0 );
}

///--------------------------------------------------------------
///						 左トリガーの値を取得
float Input::GetLeftTrigger() const {
	if(!controllerConnected_) {
		return 0.0f;
	}
	return controllerState_.Gamepad.bLeftTrigger / 255.0f;
}

///--------------------------------------------------------------
///						 右トリガーの値を取得
float Input::GetRightTrigger() const {
	if(!controllerConnected_) {
		return 0.0f;
	}
	return controllerState_.Gamepad.bRightTrigger / 255.0f;
}

///--------------------------------------------------------------
///						 左スティックのX軸の値を取得
float Input::GetLeftStickX() const {
	if(!controllerConnected_) {
		return 0.0f;
	}
	float value = static_cast<float>( controllerState_.Gamepad.sThumbLX ) / 32767.0f;
	return ( fabs(value) < stickDeadZone_ ) ? 0.0f : value;
}

///--------------------------------------------------------------
///						 左スティックのY軸の値を取得
float Input::GetLeftStickY() const {
	if(!controllerConnected_) {
		return 0.0f;
	}
	float value = static_cast<float>( controllerState_.Gamepad.sThumbLY ) / 32767.0f;
	return ( fabs(value) < stickDeadZone_ ) ? 0.0f : value;
}

///--------------------------------------------------------------
///						 右スティックのX軸の値を取得
float Input::GetRightStickX() const {
	if(!controllerConnected_) {
		return 0.0f;
	}
	float value = static_cast<float>( controllerState_.Gamepad.sThumbRX ) / 32767.0f;
	return ( fabs(value) < stickDeadZone_ ) ? 0.0f : value;
}

///--------------------------------------------------------------
///						 右スティックのY軸の値を取得
float Input::GetRightStickY() const {
	if(!controllerConnected_) {
		return 0.0f;
	}
	float value = static_cast<float>( controllerState_.Gamepad.sThumbRY ) / 32767.0f;
	return ( fabs(value) < stickDeadZone_ ) ? 0.0f : value;
}

///--------------------------------------------------------------
///						 左スティックが左に傾いているかをチェック
bool Input::IsLeftStickLeft() const {
	return GetLeftStickX() < -stickDeadZone_;
}

///--------------------------------------------------------------
///						 左スティックが右に傾いているかをチェック
bool Input::IsLeftStickRight() const {
	return GetLeftStickX() > stickDeadZone_;
}

///--------------------------------------------------------------
///						 左スティックが上に傾いているかをチェック
bool Input::IsLeftStickUp() const {
	return GetLeftStickY() > stickDeadZone_;
}

///--------------------------------------------------------------
///						 左スティックが下に傾いているかをチェック
bool Input::IsLeftStickDown() const {
	return GetLeftStickY() < -stickDeadZone_;
}

///--------------------------------------------------------------
///						 右スティックが左に傾いているかをチェック
bool Input::IsRightStickLeft() const {
	return GetRightStickX() < -stickDeadZone_;
}

///--------------------------------------------------------------
///						 右スティックが右に傾いているかをチェック
bool Input::IsRightStickRight() const {
	return GetRightStickX() > stickDeadZone_;
}

///--------------------------------------------------------------
///						 右スティックが上に傾いているかをチェック
bool Input::IsRightStickUp() const {
	return GetRightStickY() > stickDeadZone_;
}

///--------------------------------------------------------------
///						 右スティックが下に傾いているかをチェック
bool Input::IsRightStickDown() const {
	return GetRightStickY() < -stickDeadZone_;
}

///=============================================================================
///						ImGui描画
void Input::ImGuiDraw() {
	ImGui::Begin("Input");

	// キーボードの状態
	ImGui::Text("Keyboard:");
	for(int i = 0; i < 256; ++i) {
		if(keyState_[i] & 0x80) {
			ImGui::Text("Key: %d", i);
		}
	}

	// コントローラーの状態
	if(controllerConnected_) {
		ImGui::Separator();
		ImGui::Text("Controller:");
		ImGui::Text("Buttons: 0x%04X", controllerState_.Gamepad.wButtons);
		ImGui::Text("Left Trigger: %f", GetLeftTrigger());
		ImGui::Text("Right Trigger: %f", GetRightTrigger());
		ImGui::Text("Left Stick X: %f", GetLeftStickX());
		ImGui::Text("Left Stick Y: %f", GetLeftStickY());
		ImGui::Text("Right Stick X: %f", GetRightStickX());
		ImGui::Text("Right Stick Y: %f", GetRightStickY());
	} else {
		ImGui::Text("Controller not connected.");
	}

	// マウスの状態
	ImGui::Separator();
	ImGui::Text("Mouse:");
	ImGui::Text("Position: (%d, %d)", mousePos_.x, mousePos_.y);
	ImGui::Text("Movement: (%f, %f)", GetMouseMove().x, GetMouseMove().y);
	ImGui::Text("Wheel: %f", GetMouseWheel());
	ImGui::Text("Buttons: Left=%d, Right=%d, Middle=%d", mouseButtons_[0], mouseButtons_[1], mouseButtons_[2]);

	ImGui::End();
}
