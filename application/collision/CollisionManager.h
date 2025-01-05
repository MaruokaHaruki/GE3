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

///=============================================================================
///						コリジョンマネージャー
class CollisionManager {
public:
    /// \brief 初期化
    void Initialize();

    /// \brief 更新
    void Update();

    /// \brief コライダーの追加
    void AddCollider(const std::shared_ptr<Collider>& collider);

    /// \brief コライダーの削除
    void RemoveCollider(const std::shared_ptr<Collider>& collider);

private:
    // コライダーのリスト
    std::vector<std::shared_ptr<Collider>> colliders_;
};
