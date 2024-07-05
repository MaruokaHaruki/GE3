#pragma once
#include <windows.h>
#include <wrl.h>
#define DIRECTINPUT_VERSION    0x0800 //DirectInputのバージョン指定
#include <dinput.h>

///=====================================================/// 
///入力クラス
///=====================================================///
class Input {
public:
	/// ===namespace省略=== ///
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(HINSTANCE hInstance, HWND hwnd);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

private:
	/// ===キーボードのデバイス=== ///
	ComPtr<IDirectInputDevice8> keyboard;

};

