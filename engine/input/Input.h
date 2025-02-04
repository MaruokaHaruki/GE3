#pragma once
#include <windows.h>
#include <Xinput.h>
#include <dinput.h>
#pragma comment(lib, "xinput.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#include "Vector2.h"
#include "memory"
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;
///=============================================================================
///						入力クラス
class Input {
public:
	///=============================================================================
	///						メンバ関数
public:
	/**----------------------------------------------------------------------------
	* \brief  GetInstance インスタンスの取得
	* \return
	*/
	static Input* GetInstance();

	/**----------------------------------------------------------------------------
	* \brief  Initialize 初期化
	* \param  hInstance
	* \param  hwnd
	*/
	void Initialize(HINSTANCE hInstance, HWND hwnd);

	/**----------------------------------------------------------------------------
	* \brief  Update 更新
	*/
	void Update();

	///--------------------------------------------------------------
	///						 マウス系
	/**----------------------------------------------------------------------------
	* \brief  GetMouseMove マウスの移動量を取得
	* \return マウスの移動量
	*/
	Vector2 GetMouseMove() const;

	/**----------------------------------------------------------------------------
	* \brief  GetMousePositionFromCenter ウィンドウの中心からのマウスの位置を取得
	* \return ウィンドウの中心からのマウスの位置
	*/
	Vector2 GetMousePosFromWindowCenter() const;

	/**----------------------------------------------------------------------------
	* \brief  GetMouseWheel マウスホイールの移動量を取得
	* \return ホイール回転量（前回からの差分）
	*/
	float GetMouseWheel() const;

	/**----------------------------------------------------------------------------
	* \brief  PushMouseButton マウスのボタンの押下をチェック
	* \param  buttonNumber ボタン番号（0:左ボタン、1:右ボタン、2:中ボタン）
	* \return
	*/
	bool PushMouseButton(int buttonNumber) const;

	/**----------------------------------------------------------------------------
	* \brief  TriggerMouseButton マウスのボタンが押されているかをチェック
	* \param  buttonNumber ボタン番号（0:左ボタン、1:右ボタン、2:中ボタン）
	* \return
	*/
	bool TriggerMouseButton(int buttonNumber) const;
	/**----------------------------------------------------------------------------
	* \brief  OnMouseWheel マウスホイールの値を更新
	* \param  delta ホイールの回転量
	*/
	void OnMouseWheel(short delta);

	///--------------------------------------------------------------
	///						 キーボード系
	/**----------------------------------------------------------------------------
	* \brief  PushKey キーの押下をチェック
	* \param  keyCode スキャンコード（DIK_XXX）
	* \return 押されているか
	*/
	bool PushKey(int keyCode) const;

	/**----------------------------------------------------------------------------
	* \brief  TriggerKey キーのトリガーをチェック
	* \param  keyCode スキャンコード（DIK_XXX）
	* \return トリガーか?
	*/
	bool TriggerKey(int keyCode) const;

	///--------------------------------------------------------------
	///						コントローラ系
	/**----------------------------------------------------------------------------
	* \brief  PushButton コントローラーボタンの押下をチェック
	* \param  button ボタン定数（XINPUT_GAMEPAD_XXX）
	* \return 押されているか
	*/
	bool PushButton(WORD button) const;

	/**----------------------------------------------------------------------------
	* \brief  TriggerButton コントローラーボタンのトリガーをチェック
	* \param  button ボタン定数（XINPUT_GAMEPAD_XXX）
	* \return トリガーか?
	*/
	bool TriggerButton(WORD button) const;

	/**----------------------------------------------------------------------------
	* \brief  GetLeftTrigger 左トリガーの値を取得
	* \return 左トリガーの値（0.0f〜1.0f）
	*/
	float GetLeftTrigger() const;

	/**----------------------------------------------------------------------------
	* \brief  GetRightTrigger 右トリガーの値を取得
	* \return 右トリガーの値（0.0f〜1.0f）
	*/
	float GetRightTrigger() const;

	/**----------------------------------------------------------------------------
	* \brief  GetLeftStickX 左スティックのX軸の値を取得
	* \return 左スティックのX軸の値（-1.0f〜1.0f）
	*/
	float GetLeftStickX() const;

	/**----------------------------------------------------------------------------
	* \brief  GetLeftStickY 左スティックのY軸の値を取得
	* \return 左スティックのY軸の値（-1.0f〜1.0f）
	*/
	float GetLeftStickY() const;

	/**----------------------------------------------------------------------------
	* \brief  GetRightStickX 右スティックのX軸の値を取得
	* \return 右スティックのX軸の値（-1.0f〜1.0f）
	*/
	float GetRightStickX() const;

	/**----------------------------------------------------------------------------
	* \brief  GetRightStickY 右スティックのY軸の値を取得
	* \return 右スティックのY軸の値（-1.0f〜1.0f）
	*/
	float GetRightStickY() const;

	/**----------------------------------------------------------------------------
	* \brief  IsLeftStickLeft 左スティックが左に傾いているかをチェック
	* \return
	*/
	bool IsLeftStickLeft() const;

	/**----------------------------------------------------------------------------
	* \brief  IsLeftStickRight 左スティックが右に傾いているかをチェック
	* \return
	*/
	bool IsLeftStickRight() const;

	/**----------------------------------------------------------------------------
	* \brief  IsLeftStickUp 左スティックが上に傾いているかをチェック
	* \return
	*/
	bool IsLeftStickUp() const;

	/**----------------------------------------------------------------------------
	* \brief  IsLeftStickDown 左スティックが下に傾いているかをチェック
	* \return
	*/
	bool IsLeftStickDown() const;

	/**----------------------------------------------------------------------------
	* \brief  IsRightStickLeft 右スティックが左に傾いているかをチェック
	* \return
	*/
	bool IsRightStickLeft() const;

	/**----------------------------------------------------------------------------
	* \brief  IsRightStickRight 右スティックが右に傾いているかをチェック
	* \return
	*/
	bool IsRightStickRight() const;

	/**----------------------------------------------------------------------------
	* \brief  IsRightStickUp 右スティックが上に傾いているかをチェック
	* \return
	*/
	bool IsRightStickUp() const;

	/**----------------------------------------------------------------------------
	* \brief  IsRightStickDown 右スティックが下に傾いているかをチェック
	* \return
	*/
	bool IsRightStickDown() const;

	///--------------------------------------------------------------
	///						 ImGui
	/**----------------------------------------------------------------------------
	* \brief  ImGuiDraw ImGuiの描画
	*/
	void ImGuiDraw();

private:
	// シングルトンパターンのため、コンストラクタとデストラクタを非公開にする
	Input() = default;
	~Input();
	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;

	///=============================================================================
	///						メンバ変数
private:
	//========================================
	// HWNDを保持
	HWND hwnd_ = nullptr;
	//========================================
	// DirectInput関連
	HINSTANCE hInstance_ = nullptr;
	IDirectInput8* directInput_ = nullptr;
	IDirectInputDevice8* keyboardDevice_ = nullptr;

	//========================================
	// マウスの状態
	POINT mousePos_ = {};
	POINT mousePosPrev_ = {};
	float mouseWheel_ = 0.0f;
	float mouseWheelPrev_ = 0.0f;
	bool mouseButtons_[3] = {};
	bool mouseButtonsPrev_[3] = {};
	//========================================
	// キーボードの状態
	BYTE keyState_[256] = {};
	BYTE keyStatePrev_[256] = {};
	//========================================
	// コントローラーの状態
	XINPUT_STATE controllerState_ = {};
	XINPUT_STATE controllerStatePrev_ = {};
	bool controllerConnected_ = false;
	//========================================
	// デッドゾーン
	float stickDeadZone_ = 0.2f;
};
