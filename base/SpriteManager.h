///===================================================================///
///						SpriteManagerクラス
///===================================================================///
//NOTE:スプライト共通部
#include "DirectXManager.h"

#pragma once
class SpriteManager {
public:
	///-------------------------------------------/// 
	/// メンバ関数
	///-------------------------------------------///

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXManager* dxManager);

	/// <summary>
	/// 共通描画設定
	/// </summary>
	void CommonDrawSetup();

private:

	/// <summary>
	/// ルートシグネチャーの作成
	/// </summary>
	void CreateRootSignature();

	/// <summary>
	/// グラフィックスパイプラインの作成
	/// </summary>
	void CreateGraphicsPipeline();

public:
	///-------------------------------------------///  
	///ゲッター・セッター
	///-------------------------------------------///
	/// ===DirectXManager=== ///
	//Getter
	DirectXManager* GetDXManager() const { return dxManager_; }



private:
	///-------------------------------------------/// 
	///メンバ変数
	///-------------------------------------------///

	/// ===DirectXManager.Pointer=== ///
	DirectXManager* dxManager_;

	/// ===RootSignature=== ///
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

	/// ===グラフィックスパイプライン=== ///
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;
};

