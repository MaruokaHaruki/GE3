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

	/// <summary>
	/// 左スティックのX軸の値を取得
	/// </summary>
	/// <returns>左スティックのX軸の値</returns>
	float GetLeftStickX() const;

	/// <summary>
	/// 左スティックのY軸の値を取得
	/// </summary>
	/// <returns>左スティックのY軸の値</returns>
	float GetLeftStickY() const;

	/// <summary>
	/// 右スティックのX軸の値を取得
	/// </summary>
	/// <returns>右スティックのX軸の値</returns>
	float GetRightStickX() const;

	/// <summary>
	/// 右スティックのY軸の値を取得
	/// </summary>
	/// <returns>右スティックのY軸の値</returns>
	float GetRightStickY() const;

	/// <summary>
	/// デッドゾーンの値を設定
	/// </summary>
	/// <param name="deadZone">デッドゾーンの値 (0.0 ~ 1.0)</param>
	//void SetDeadZone(float deadZone);

	/// <summary>
	/// 左スティックが左に傾いているかをチェック
	/// </summary>
	/// <returns>左に傾いているか</returns>
	bool IsLeftStickLeft() const;

	/// <summary>
	/// 左スティックが右に傾いているかをチェック
	/// </summary>
	/// <returns>右に傾いているか</returns>
	bool IsLeftStickRight() const;

	/// <summary>
	/// 左スティックが上に傾いているかをチェック
	/// </summary>
	/// <returns>上に傾いているか</returns>
	bool IsLeftStickUp() const;

	/// <summary>
	/// 左スティックが下に傾いているかをチェック
	/// </summary>
	/// <returns>下に傾いているか</returns>
	bool IsLeftStickDown() const;

	/// <summary>
	/// 右スティックが左に傾いているかをチェック
	/// </summary>
	/// <returns>左に傾いているか</returns>
	bool IsRightStickLeft() const;

	/// <summary>
	/// 右スティックが右に傾いているかをチェック
	/// </summary>
	/// <returns>右に傾いているか</returns>
	bool IsRightStickRight() const;

	/// <summary>
	/// 右スティックが上に傾いているかをチェック
	/// </summary>
	/// <returns>上に傾いているか</returns>
	bool IsRightStickUp() const;

	/// <summary>
	/// 右スティックが下に傾いているかをチェック
	/// </summary>
	/// <returns>下に傾いているか</returns>
	bool IsRightStickDown() const;

	/// <summary>
	///	ImGui描画
	/// </summary>
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

	/// <summary>
	/// スティックのデッドゾーンを適用
	/// </summary>
	/// <param name="value">スティックの値</param>
	/// <param name="deadZone">デッドゾーンの値</param>
	/// <returns>デッドゾーンが適用された値</returns>
	float ApplyDeadZone(LONG value) const;

	/**----------------------------------------------------------------------------
	* \brief  NormalizeStickValue スティックの値を -1.0 〜 1.0 に正規化する関数
	* \param  value
	* \return
	*/
	float NormalizeStickValue(float value)const;

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

	/// ===スティックのデッドゾーン=== ///
	float deadZone = 0.2f; // デフォルトのデッドゾーンを0.2に設定
};

