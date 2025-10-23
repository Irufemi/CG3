#include "Player.h"
#include <cmath>
#include "engine/Input/InputManager.h"
#include "camera/Camera.h"
#include "3D/SphereClass.h"
#include <dinput.h>


// 画面座標(sx,sy)をZ=targetZ平面上のワールド座標へ逆変換
static Vector3 ScreenToWorldOnZ(const Camera* cam, const Vector2& screen, float targetZ) {

	Matrix4x4 view = cam->GetViewMatrix();
	Matrix4x4 proj = cam->GetPerspectiveFovMatrix();
	Matrix4x4 vp = cam->GetViewportMatrix();

	// v * View * Proj * Viewport の逆行列
	Matrix4x4 vpv = Math::Multiply(view, Math::Multiply(proj, vp));
	Matrix4x4 inv = Math::Inverse(vpv);

	Vector3 p0 = Math::Transform(Vector3{ screen.x, screen.y, 0.0f }, inv);
	Vector3 p1 = Math::Transform(Vector3{ screen.x, screen.y, 1.0f }, inv);
	Vector3 dir = Math::Subtract(p1, p0);

	float denom = dir.z;
	if (std::fabs(denom) < 1e-6f) {
		return p0; // レイがZ平面とほぼ平行
	}
	float t = (targetZ - p0.z) / denom;
	return Math::Add(p0, Math::Multiply(t, dir));
}

// 画面半径[pixels]をZ=targetZ平面でのワールド半径に変換
static float ScreenRadiusToWorld(const Camera* cam, const Vector2& center, float radiusPx, float targetZ) {
	Vector3 wc = ScreenToWorldOnZ(cam, center, targetZ);
	Vector3 wx = ScreenToWorldOnZ(cam, Vector2{ center.x + radiusPx, center.y }, targetZ);
	Vector2 d = Math::Subtract(Vector2{ wx.x, wx.y }, Vector2{ wc.x, wc.y });
	return Math::Length(d);
}


void Player::Initialize (InputManager* inputManager, Camera* camera) {

	camera_ = camera;
	inputManager_ = inputManager;

	pos_ = { 100.0f, 400.0f };
	radius_ = { 30.0f, 30.0f };
	velocity_ = {-1.0f, 0.0f};

	cannonPos_ = { pos_.x, pos_.y - radius_.y };
	cannonRadius_ = { 12.0f, 24.0f };
	cannonOffset_ = { 0.0f, -30.0f };
	angle_ = 0.0f;
	rad_ = 0.0f;
	sinf_ = 0.0f;
	cosf_ = 0.0f;

	for (auto& b : bullet) {
		b.Initialize (pos_, sinf_, cosf_);
	}

	sphere_ = std::make_unique<SphereClass>();
	sphere_->SetInfo(Sphere{ Vector3{pos_.x,pos_.y,0.0f},radius_.x });
	sphere_->Initialize(camera);

}

void Player::Jump () {
	if (inputManager_->IsKeyPressedDIK(DIK_SPACE)) {
		if (pos_.x <= 250.0f) {
			velocity_.x = 4.0f;
			velocity_.y = 6.0f;
		}
		else if (pos_.x >= 250.0f) {
			velocity_.x = -4.0f;
			velocity_.y = 6.0f;
		}
	}
}

void Player::Rotate () {
	if (inputManager_->IsKeyDownDIK(DIK_A)) {
		angle_ -= 5.0f;
	}
	if (inputManager_->IsKeyDownDIK(DIK_D)) {
		angle_ += 5.0f;
	}

	// --- ラジアン変換 ---
	rad_ = angle_ * (3.14159265f / 180.0f);

	// --- 回転を適用（オフセットを回転） ---
	Vector2 rotatedOffset;
	rotatedOffset.x = cannonOffset_.x * cosf (rad_) - cannonOffset_.y * sinf (rad_);
	rotatedOffset.y = cannonOffset_.x * sinf (rad_) + cannonOffset_.y * cosf (rad_);

	// --- プレイヤー位置を中心に戻す ---
	cannonPos_.x = pos_.x + rotatedOffset.x;
	cannonPos_.y = pos_.y + rotatedOffset.y;
}

void Player::Fire () {
	if (inputManager_->IsKeyPressedDIK(DIK_SPACE)) {
		for (auto& b : bullet) {
			if (!b.GetIsActive ()) {
				b.Initialize (cannonPos_, sinf (rad_), cosf (rad_));
				b.SetIsActive ();
				break;
			}
		}
	}
}

void Player::SpeedCalculation () {
	if (pos_.x - radius_.x <= 0.0f || pos_.x + radius_.x >= 500.0f) {
		velocity_.x *= -1.0f;
	}
	if (pos_.y - radius_.y <= 0.0f) {
		velocity_.y = 0.0f;
	}

	velocity_.y += kGravity * deltaTime;
}

void Player::Input () {
	Jump ();
	Fire ();

}

void Player::Update () {
	//座標更新
	pos_.x += velocity_.x;
	pos_.y -= velocity_.y;
	//大砲
	Rotate ();
	for (auto& b : bullet) {
		b.Update ();
	}
}

void Player::DrawSet() {
	Vector3 wc = ScreenToWorldOnZ(camera_, pos_, 0.0f);
	float wr = ScreenRadiusToWorld(camera_, pos_, radius_.x, 0.0f);
	sphere_->SetInfo(Sphere{ wc, wr });
	// 2Dの角度(angle_ → rad_)をZ回転として適用
	sphere_->SetRotate(Vector3{ 0.0f, 0.0f, -rad_ });
	sphere_->Update("PlayerSphere");


	// 回転行列を使って4点を回す
	Vector2 local[4] = {
		{-cannonRadius_.x, -cannonRadius_.y},
		{ cannonRadius_.x, -cannonRadius_.y},
		{-cannonRadius_.x,  cannonRadius_.y},
		{ cannonRadius_.x,  cannonRadius_.y},
	};

	for (int i = 0; i < 4; i++) {
		float x = local[i].x;
		float y = local[i].y;
		local[i].x = x * cosf(rad_) - y * sinf(rad_);
		local[i].y = x * sinf(rad_) + y * cosf(rad_);
		local[i].x += cannonPos_.x;
		local[i].y += cannonPos_.y;
	}



}

void Player::Draw () {
	//player
	
	sphere_->Draw();
	
	//Shape::DrawEllipse (pos_.x, pos_.y, radius_.x, radius_.y, 0.0f, WHITE, kFillModeSolid);
	//cannon

	/*Shape::DrawQuad (
		local[0].x, local[0].y,
		local[1].x, local[1].y,
		local[2].x, local[2].y,
		local[3].x, local[3].y,
		0, 0, 0, 0, 0, BLACK
	);*/

	//弾
	for (auto& b : bullet) {
		b.Draw ();
	}
}