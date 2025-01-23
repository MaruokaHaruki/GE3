#pragma once
#include <windows.h>
#include <wrl.h>
#define DIRECTINPUT_VERSION    0x0800 //DirectInputのバージョン指定
#include <dinput.h>
#include "Vector2.h"

///=============================================================================
///						入力クラス
class Input {
public:
	/// ===namespace省略=== ///
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	/// <summary>
	/// インスタンスの取得
	/// </summary>
	/// <returns>Inputのインスタンス</returns>
	static Input *GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(HINSTANCE hInstance, HWND hwnd);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();


	///--------------------------------------------------------------
	///						 マウス系
	/**----------------------------------------------------------------------------
	 * \brief  GetMouseMove 
	 * \return 
	 */
	Vector2 GetMouseMove()const;

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

	///--------------------------------------------------------------
	///						 キーボード系
	/*
	 * \brief  PushKey キーの押下をチェック
	 * \param  keyNumber "キー番号"(DIK_0等)
	 * \return 押されているか
	 */
	bool PushKey(BYTE keyNumber);
	/**----------------------------------------------------------------------------
	 * \brief  TriggerKey キーのトリガーをチェック
	 * \param  keyNumber "キー番号"(DIK_0等)
	 * \return トリガーか?
	 */
	bool TriggerKey(BYTE keyNumber);

	///--------------------------------------------------------------
	///						コントローラ系
	/**----------------------------------------------------------------------------
	 * \brief  PushButton コントローラーボタンの押下をチェック
	 * \param  buttonNumber ボタン番号
	 * \return 押されているか
	 */
	bool PushButton(BYTE buttonNumber);
	/**----------------------------------------------------------------------------
	 * \brief  TriggerButton コントローラーボタンのトリガーをチェック
	 * \param  buttonNumber ボタン番号
	 * \return
	 */
	bool TriggerButton(BYTE buttonNumber);
	/**----------------------------------------------------------------------------
	 * \brief  GetLeftStickX 左スティックのX軸の値を取得
	 * \return 左スティックのX軸の値
	 */
	float GetLeftStickX() const;
	/**----------------------------------------------------------------------------
	 * \brief  GetLeftStickY 左スティックのY軸の値を取得
	 * \return 左スティックのY軸の値
	 */
	float GetLeftStickY() const;
	/**----------------------------------------------------------------------------
	 * \brief  GetRightStickX 右スティックのX軸の値を取得
	 * \return 右スティックのX軸の値
	 */
	float GetRightStickX() const;
	/**----------------------------------------------------------------------------
	 * \brief  GetRightStickY
	 * \return
	*/
	float GetRightStickY() const;
	/**
	 * \brief デッドゾーンの設定
	 */
	//void SetDeadZone(float deadZone);
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

	/**----------------------------------------------------------------------------
	 * \brief  ImGuiDraw ImGuiの描画
	 */
	void ImGuiDraw();

	// コントローラーボタンの定数
	static const BYTE BUTTON_A = 0;
	static const BYTE BUTTON_B = 1;
	static const BYTE BUTTON_X = 2;
	static const BYTE BUTTON_Y = 3;
	static const BYTE BUTTON_LB = 4;
	static const BYTE BUTTON_RB = 5;
	static const BYTE BUTTON_BACK = 6;
	static const BYTE BUTTON_START = 7;
	static const BYTE BUTTON_LS = 8; // 左スティック押し込み
	static const BYTE BUTTON_RS = 9; // 右スティック押し込み

private:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Input() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Input() = default;

	/// <summary>
	/// コピーコンストラクタを削除
	/// </summary>
	Input(const Input &) = delete;

	/// <summary>
	/// 代入演算子を削除
	/// </summary>
	Input &operator=(const Input &) = delete;

	/**----------------------------------------------------------------------------
	 * \brief  ApplyDeadZone デッドゾーンを適用する関数
	 * \param  value デッドゾーンを適用する値
	 * \return
	 */
	float ApplyDeadZone(LONG value) const;

	/**----------------------------------------------------------------------------
	* \brief  NormalizeStickValue スティックの値を -1.0 〜 1.0 に正規化する関数
	* \param  value
	* \return
	*/
	float NormalizeStickValue(float value)const;

	///=============================================================================
	///						メンバ変数
private:
	//========================================
	// DirectInputのインスタンス
	ComPtr<IDirectInput8>directInput = nullptr;

	//========================================
	// マウスのデバイス
	ComPtr<IDirectInputDevice8> mouse_;
	// マウスの現在の状態
	DIMOUSESTATE2 mouseState_ = {};
	// マウスの前回の状態
	DIMOUSESTATE2 mouseStatePrev_ = {};

	//========================================
	// キーボードのデバイス
	ComPtr<IDirectInputDevice8> keyboard = nullptr;
	//全キーの状態
	BYTE key[256] = {};
	//前回の全キーの状態
	BYTE keyPre[256] = {};

	//========================================
	// コントローラーのデバイス
	ComPtr<IDirectInputDevice8> controller;
	// 全ボタンの状態
	DIJOYSTATE2 controllerState = {};
	// 前回の全ボタンの状態
	DIJOYSTATE2 controllerStatePre = {};
	// コントローラーが接続されているかどうか
	bool controllerConnected = false;
	// スティックのデッドゾーン
	float deadZone = 0.2f; // デフォルトのデッドゾーンを0.2に設定
};

