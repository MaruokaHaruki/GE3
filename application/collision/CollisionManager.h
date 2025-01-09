/*********************************************************************
 * \file   CollisionManager.h
 * \brief  
 * 
 * \author Harukichimaru
 * \date   January 2025
 * \note   
 *********************************************************************/
#pragma once
#include <vector>
#include <memory>
#include "Collider.h"
#include "BaseObject.h"
#include "Object3d.h"

///=============================================================================
///						コリジョンマネージャー
class CollisionManager {
public:
    /// \brief 初期化
    void Initialize(Object3d *object3d);

    /// \brief 更新
    void Update();

	/// \brief 描画
	void Draw();

	/// @brief ImGuiの描画
	void DrawImGui();

	/**----------------------------------------------------------------------------
	* \brief  Reset リセット
	* \note
	*/
	void Reset();

	/**----------------------------------------------------------------------------
	* \brief  AddCollider 当たり判定を追加
	* \param  characterBase 追加する当たり判定
	* \note
	*/
	void AddCollider(BaseObject* baseObj);

	/**----------------------------------------------------------------------------
	* \brief  CheckColliderPair 当たり判定同士をチェック
	* \param  characterA
	* \param  characterB
	* \note
	*/
	void CheckColliderPair(BaseObject* baseObjA, BaseObject* baseObjB);

	/**----------------------------------------------------------------------------
	* \brief  CheckAllCollisions すべての当たり判定をチェック
	* \note
	*/
	void CheckAllCollisions();

	///--------------------------------------------------------------
	///						 メンバ変数
private:

	struct PairHash {
		template <class T1, class T2>
		std::size_t operator()(const std::pair<T1, T2>& pair) const {
			auto hash1 = std::hash<T1>{}(pair.first);
			auto hash2 = std::hash<T2>{}(pair.second);
			return hash1 ^ hash2; // シンプルなXORでハッシュを計算
		}
	};

	/// ===コライダー=== ///
	std::list<BaseObject*> Objects_;

	std::unordered_set<BaseObject*> collidedObjects_; // 衝突したオブジェクトを追跡するセット

	std::unordered_set<std::pair<BaseObject*, BaseObject*>, PairHash> collidedPairs_; // 衝突済みペア

	//========================================
	// 判定描画
	bool isHitDraw_ = false;
	//========================================
	// Object3D
	Object3d *object3d_ = nullptr;
};
