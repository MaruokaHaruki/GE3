/*********************************************************************
 * \file   Camera.h
 * \brief  カメラクラス
 *
 * \author Harukichimaru
 * \date   December 2024
 * \note
 *********************************************************************/
#pragma once
#include "Transform.h"
#include "Matrix4x4.h"

///=============================================================================
///						
class Camera {

	///--------------------------------------------------------------
	///							メンバ関数
public:

	/// \brief デフォルトコンストラクタ
	Camera();

	/// \brief 初期化	
	void Initialize();

	/// \brief 更新
	void Update();

	/// \brief 描画 
	void Draw();

	///--------------------------------------------------------------
	///							静的メンバ関数
private:

	///--------------------------------------------------------------
	///							入出力関数
public:

	/**----------------------------------------------------------------------------
	 * \brief  SetWorldMatrix ワールド行列の設定
	 * \param  worldMatrix ワールド行列
	 */
	const Matrix4x4& GetWorldMatrix() const { return worldMatrix_; }

	/**----------------------------------------------------------------------------
	 * \brief  SetViewMatrix ビュー行列の設定
	 * \param  viewMatrix ビュー行列
	 */
	const Matrix4x4& GetViewMatrix() const { return viewMatrix_; }

	/**----------------------------------------------------------------------------
	 * \brief  SetProjectionMatrix プロジェクション行列の設定
	 * \param  projectionMatrix プロジェクション行列
	 */
	const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix_; }

	/**----------------------------------------------------------------------------
	 * \brief  SetViewProjectionMatrix ビュープロジェクション行列の設定
	 * \param  viewProjectionMatrix ビュープロジェクション行列
	 */
	const Matrix4x4& GetViewProjectionMatrix() const { return viewProjectionMatrix_; }

	/**----------------------------------------------------------------------------
	 * \brief  SetTransform トランスフォームの設定
	 * \param  transform トランスフォーム
	 */
	void SetTransform(const Transform& transform) { transform_ = transform; }
	/*
	 * \brief  GetTransform　 トランスフォームの取得
	 * \return translate トランスフォーム
	 */
	Transform GetTransform() const { return transform_; }

	/**----------------------------------------------------------------------------
	 * \brief  SetTranslate 移動の設定
	 * \param  translate 移動
	 */
	void SetTranslate(const Vector3& translate) { transform_.translate = translate; }
	/*
	 * \brief  GetTranslate 移動の取得
	 * \return translate 移動
	 */
	const Vector3& GetTranslate() const { return transform_.translate; }

	/**----------------------------------------------------------------------------
	 * \brief  SetRotate 回転の設定
	 * \param  rotate 回転
	 */
	void SetRotate(const Vector3& rotate) { transform_.rotate = rotate; }
	/*
	 * \brief  GetRotate 回転の取得
	 * \return 
	 */
	const Vector3& GetRotate() const { return transform_.rotate; }

	/**----------------------------------------------------------------------------
	 * \brief  SetFovY FovYの設定
	 * \param  FovY 
	 */
	void SetFovY(float fovY) { horizontalFieldOfView_ = fovY; }
	/*
	 * \brief  GetFovY FovYの取得
	 * \return FovY
	 */
	float GetFovY() const { return horizontalFieldOfView_; }

	/**----------------------------------------------------------------------------
	 * \brief  SetNearClip ニアクリップの設定
	 * \param  nearClip ニアクリップ
	 */
	void SetAspectRatio(float aspectRatio) { aspectRatio_ = aspectRatio; }
	/*
	 * \brief  GetNearClip ニアクリップの取得
	 * \return nearClip ニアクリップ
	 */
	float GetAspectRatio() const { return aspectRatio_; }

	/**----------------------------------------------------------------------------
	 * \brief  SetNearClip ニアクリップの設定
	 * \param  nearClip ニアクリップ
	 */
	void SetNearClip(float nearClip) { nearClipRange_ = nearClip; }
	/*
	 * \brief  GetNearClip
	 * \return nearClip
	 */
	float GetNearClip() const { return nearClipRange_; }

	/**----------------------------------------------------------------------------
	 * \brief  SetFarClip ファークリップの設定
	 * \param  farClip ファークリップ
	 */
	void SetFarClip(float farClip) { farClipRange_ = farClip; }
	/*
	 * \brief  GetFarClip ファークリップの取得
	 * \return farClip ファークリップ
	 */
	float GetFarClip() const { return farClipRange_; }


	///--------------------------------------------------------------
	///							メンバ変数
private:
	//---------------------------------------
	// カメラのトランスフォーム
	Transform transform_;

	//---------------------------------------
	// ワールド行列
	Matrix4x4 worldMatrix_;
	// ビュー行列
	Matrix4x4 viewMatrix_;

	//---------------------------------------
	// プロジェクション行列関連データ
	Matrix4x4 projectionMatrix_;
	// 水平視野角(FOV)
	float horizontalFieldOfView_;
	// アスペクト比
	float aspectRatio_;
	// ニアクリップ
	float nearClipRange_;
	// ファークリップ
	float farClipRange_;

	//---------------------------------------
	// ビュープロジェクション行列
	Matrix4x4 viewProjectionMatrix_;

};

