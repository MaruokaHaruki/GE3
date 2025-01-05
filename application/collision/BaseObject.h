#pragma once
#include <memory>
#include <unordered_set>
#include "Collider.h"

///=============================================================================
///						基底オブジェクト
class BaseObject {
public:
    /// \brief 仮想デストラクタ
    virtual ~BaseObject() = default;

    /// \brief 初期化
    virtual void Initialize();

    /// \brief 更新
    virtual void Update();

    /// \brief 描画
    virtual void Draw();

    /// \brief 衝突開始時の処理
    virtual void OnCollisionEnter(BaseObject* other);

    /// \brief 衝突継続時の処理
    virtual void OnCollisionStay(BaseObject* other);

    /// \brief 衝突終了時の処理
    virtual void OnCollisionExit(BaseObject* other);

    /// \brief コライダーの取得
    std::shared_ptr<Collider> GetCollider() const { return collider_; }

    /// \brief コライダーの設定
    void SetCollider(std::shared_ptr<Collider> collider) { collider_ = collider; }

    /// \brief 衝突中のオブジェクトのセットの取得
    std::unordered_set<BaseObject*>& GetCollidingObjects() { return collidingObjects_; }

protected:
    // コライダー
    std::shared_ptr<Collider> collider_;

    // 衝突中のオブジェクトのセット
    std::unordered_set<BaseObject*> collidingObjects_;
};

