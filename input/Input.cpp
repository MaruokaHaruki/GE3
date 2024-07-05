#include "Input.h"
#include <cassert>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

///=====================================================/// 
///初期化
///=====================================================///
void Input::Initialize(HINSTANCE hInstance, HWND hwnd) {

	HRESULT result;

	/// ===DirectInputのインスタンス生成=== ///
	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));
	/// ===キーボードデバイス再生=== ///
	result = directInput->CreateDevice(GUID_SysKeyboard,&keyboard,NULL);
	assert(SUCCEEDED(result));
	/// ===入力データ形式のセット=== ///
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));
	/// ===排他抑制レベルのセット=== ///
	result = keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
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
