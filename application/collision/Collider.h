#pragma once
#include "Vector3.h"
#include <memory>

class BaseObject;

///=============================================================================
///						コライダー
class Collider {
public:
    /// \brief コンストラクタ
    Collider(BaseObject* owner);

    /// \brief オーナーオブジェクトの取得
    BaseObject* GetOwner() const { return owner_; }

    /// \brief 位置の取得
    const Vector3& GetPosition() const { return position_; }

    /// \brief 位置の設定
    void SetPosition(const Vector3& position) { position_ = position; }

    /// \brief 半径の取得
    float GetRadius() const { return radius_; }

    /// \brief 半径の設定
    void SetRadius(float radius) { radius_ = radius; }

private:
    // オーナーオブジェクト
    BaseObject* owner_;

    // 位置
    Vector3 position_ = { 0.0f, 0.0f, 0.0f };

    // 半径（球体コライダーを想定）
    float radius_ = 1.0f;
};
