#include "Collider.h"

///=============================================================================
///						円同士の判定
bool Collider::Intersects(const Collider& other) const {
    // 2つの球体間の距離の二乗を計算
    Vector3 diff = position_ - other.position_;
    float distanceSquared = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

    // 半径の合計の二乗を計算
    float radiusSum = radius_ + other.radius_;
    float radiusSumSquared = radiusSum * radiusSum;

    // 距離の二乗が半径の合計の二乗以下であれば衝突
    return distanceSquared <= radiusSumSquared;
}