#include "Input.h"
#include <cassert>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

/// <summary>
/// インスタンスの取得
/// </summary>
/// <returns>Inputのインスタンス</returns>
Input* Input::GetInstance() {
	static Input instance;
	return &instance;
}

///=====================================================/// 
///初期化
///=====================================================///
void Input::Initialize(HINSTANCE hInstance, HWND hwnd) {

	HRESULT result;

	/// ===DirectInputのインスタンス生成=== ///
	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
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
	if (SUCCEEDED(result)) {
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

///=====================================================///  
///更新
///=====================================================///
void Input::Update() {

	/// ===前回のキー入力を保存=== ///
	memcpy(keyPre, key, sizeof(key));

	/// ===キーボード情報の取得開始=== ///
	keyboard->Acquire();
	/// ===全キーの取得=== ///
	keyboard->GetDeviceState(sizeof(key), key);

	if (controllerConnected) {
		/// ===前回のコントローラー入力を保存=== ///
		controllerStatePre = controllerState;

		/// ===コントローラー情報の取得開始=== ///
		controller->Acquire();
		/// ===全ボタンの取得=== ///
		controller->GetDeviceState(sizeof(DIJOYSTATE2), &controllerState);
	}
}

///=====================================================/// 
///キーの押下をチェック
///=====================================================///
bool Input::PushKey(BYTE keyNumber) {
	/// ===指定キーを押していればtrueを返す=== ///
	if (key[keyNumber]) {
		return true;
	}

	/// ===そうでなければfalse=== ///
	return false;
}

bool Input::TriggerKey(BYTE keyNumber) {
	/// ===前回は押しておらず、今回は押している場合trueを返す=== ///
	if (key[keyNumber] && !keyPre[keyNumber]) {
		return true;
	}

	return false;
}

///=====================================================/// 
///コントローラーボタンの押下をチェック
///=====================================================///
bool Input::PushButton(BYTE buttonNumber) {
	if (!controllerConnected) {
		return false;
	}

	/// ===指定ボタンを押していればtrueを返す=== ///
	if (controllerState.rgbButtons[buttonNumber]) {
		return true;
	}

	/// ===そうでなければfalse=== ///
	return false;
}

bool Input::TriggerButton(BYTE buttonNumber) {
	if (!controllerConnected) {
		return false;
	}

	/// ===前回は押しておらず、今回は押している場合trueを返す=== ///
	if (controllerState.rgbButtons[buttonNumber] && !controllerStatePre.rgbButtons[buttonNumber]) {
		return true;
	}

	return false;
}

