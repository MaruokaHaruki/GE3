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

//========================================
// グリッドのセル
struct PairHash {
	template <class T1, class T2>
	std::size_t operator()(const std::pair<T1, T2>& pair) const {
		auto hash1 = std::hash<T1>{}(pair.first);
		auto hash2 = std::hash<T2>{}(pair.second);
		return hash1 ^ hash2; // シンプルなXORでハッシュを計算
	}
};

//========================================
// グリッドのセル
struct GridCell {
	std::vector<BaseObject*> objects;
};

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
	//void CheckColliderPair(BaseObject* baseObjA, BaseObject* baseObjB);

	/**----------------------------------------------------------------------------
	* \brief  CheckAllCollisions すべての当たり判定をチェック
	* \note
	*/
	void CheckAllCollisions();

	///--------------------------------------------------------------
	///						 静的メンバ関数
private:
	/**----------------------------------------------------------------------------
	 * \brief  GetGridIndex グリッドのインデックスを取得
	 * \param  position 位置
	 * \return 
	 */
	int GetGridIndex(const Vector3& position) const;

	/**----------------------------------------------------------------------------
	 * \brief  CheckCollisionsInCell セル内の衝突をチェック
	 * \param  cell セル
	 */
	void CheckCollisionsInCell(const GridCell& cell);

	/**----------------------------------------------------------------------------
	 * \brief  CheckCollisionsBetweenCells セル間の衝突をチェック
	 * \param  cellA
	 * \param  cellB
	 */
	void CheckCollisionsBetweenCells(const GridCell& cellA, const GridCell& cellB);



	///--------------------------------------------------------------
	///						 メンバ変数
private:
	//========================================
	// グリッド
	std::unordered_map<int, GridCell> grid_;
	// グリッドのセルのサイズ
	float cellSize_ = 64.0f;

	//========================================
	// 当たり判定
	std::list<BaseObject*> Objects_;
	// 衝突したオブジェクトを追跡するセット
	std::unordered_set<BaseObject*> collidedObjects_;
	// 衝突済みペア
	std::unordered_set<std::pair<BaseObject*, BaseObject*>, PairHash> collidedPairs_;

	//========================================
	// 判定描画
	bool isHitDraw_ = false;
	//========================================
	// Object3D
	Object3d *object3d_ = nullptr;
};
