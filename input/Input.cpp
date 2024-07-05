#include "Input.h"
#include <cassert>
#include <wrl.h>
using namespace Microsoft::WRL;
#define DIRECTINPUT_VERSION    0x0800 //DirectInputのバージョン指定
#include <dinput.h>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")


void Input::Initialize(HINSTANCE hInstance, HWND hwnd) {

	HRESULT result;

	/// ===DirectInputのインスタンス生成=== ///
	ComPtr<IDirectInput8>directInput = nullptr;
	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));
	/// ===キーボードデバイス再生=== ///
	ComPtr<IDirectInputDevice8> keyboard;
	result = directInput->CreateDevice(GUID_SysKeyboard,&keyboard,NULL);
	assert(SUCCEEDED(result));
	/// ===入力データ形式のセット=== ///
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));
	/// ===排他抑制レベルのセット=== ///
	result = keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void Input::Update() {
}
