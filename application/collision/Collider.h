#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include <memory>

class BaseObject;

///=============================================================================
///						コライダー
class Collider {
public:
    /// \brief 位置の取得
    Vector3& GetPosition()  { return position_; }

    /// \brief 位置の設定
    void SetPosition(const Vector3& position) { position_ = position; }

    /// \brief 半径の取得
    float GetRadius() const { return radius_; }

    /// \brief 半径の設定
    void SetRadius(float radius) { radius_ = radius; }

    /**----------------------------------------------------------------------------
    * \brief  Intersects
    * \param  other
    * \return
    * \note
    */
    bool Intersects(const Collider& other) const;

private:
    // 位置
    Vector3 position_ = { 0.0f, 0.0f, 0.0f };

    // 半径（球体コライダーを想定）
    float radius_ = 1.0f;

    // 色
    Vector4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
};
