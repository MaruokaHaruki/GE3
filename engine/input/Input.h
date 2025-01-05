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

	/// <summary>
	/// キーの押下をチェック
	/// </summary>
	/// <param name="keyNumber">キー番号</param>
	/// <returns>押されているか</returns>
	bool PushKey(BYTE keyNumber);

	/// <summary>
	/// キーのトリガーをチェック
	/// </summary>
	/// <param name="keyNumber">"キー番号"(DIK_0等)</param>
	/// <returns>トリガーか?</returns>
	bool TriggerKey(BYTE keyNumber);

	/// <summary>
	/// コントローラーボタンの押下をチェック
	/// </summary>
	/// <param name="buttonNumber">ボタン番号</param>
	/// <returns>押されているか</returns>
	bool PushButton(BYTE buttonNumber);

	/// <summary>
	/// コントローラーボタンのトリガーをチェック
	/// </summary>
	/// <param name="buttonNumber">ボタン番号</param>
	/// <returns>トリガーか?</returns>
	bool TriggerButton(BYTE buttonNumber);

private:
	/// ===キーボードのデバイス=== ///
	ComPtr<IDirectInputDevice8> keyboard;

	/// ===コントローラーのデバイス=== ///
	ComPtr<IDirectInputDevice8> controller;

	/// ===DirectInputのインスタンス=== ///
	ComPtr<IDirectInput8>directInput = nullptr;

	/// ===全キーの状態=== ///
	BYTE key[256] = {};

	/// ===前回の全キーの状態=== ///
	BYTE keyPre[256] = {};

	/// ===全ボタンの状態=== ///
	DIJOYSTATE2 controllerState = {};

	/// ===前回の全ボタンの状態=== ///
	DIJOYSTATE2 controllerStatePre = {};

	/// ===コントローラーが接続されているかどうか=== ///
	bool controllerConnected = false;
};
