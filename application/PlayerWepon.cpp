#include "PlayerWepon.h"
#include "Player.h"

///=============================================================================
///						初期化
void PlayerWepon::Initialize() {
	//========================================
	// 当たり判定との同期
	BaseObject::Initialize(transform.translate, 0.3f);
}

void PlayerWepon::Update() {
	//========================================
	// 当たり判定との同期
	BaseObject::Update(transform.translate);
}

void PlayerWepon::Draw() {
}

void PlayerWepon::ImGuiDraw() {
}

void PlayerWepon::OnCollisionEnter(BaseObject *other) {
	if(dynamic_cast<Player *>( other ) != nullptr) {}
}

void PlayerWepon::OnCollisionStay(BaseObject *other) {
	if(dynamic_cast<Player *>( other ) != nullptr) {}
}

void PlayerWepon::OnCollisionExit(BaseObject *other) {
	if(dynamic_cast<Player *>( other ) != nullptr) {}
}
