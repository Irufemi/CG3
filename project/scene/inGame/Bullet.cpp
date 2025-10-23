#include "Bullet.h"

const float kGravity = -9.8f;
const float deltaTime = 1.0f / 60.0f;

void Bullet::Initialize (Vector2 pos, float sin, float cos) {
	pos_ = pos;
	radius_ = { 20.0f, 20.0f };
	velocity_ = { sin * 10.0f, -cos * 10.0f }; // 発射方向に速度を設定
	isActive_ = false;
}

void Bullet::Update () {
	if (isActive_) {
		velocity_.y -= kGravity * deltaTime;

		pos_.x += velocity_.x;
		pos_.y += velocity_.y;
	}
}

void Bullet::Draw () {
	if (isActive_) {
		//Shape::DrawEllipse (pos_.x, pos_.y, radius_.x, radius_.y, 0.0f, RED, kFillModeSolid);
	}
}