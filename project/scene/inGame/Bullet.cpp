#include "Bullet.h"
#include <cmath>
#include "camera/Camera.h"
#include "function/Math.h"

// 画面座標(sx,sy)をZ=targetZ平面上のワールド座標へ逆変換（Player と同等）
static Vector3 ScreenToWorldOnZ(const Camera* cam, const Vector2& screen, float targetZ) {
    Matrix4x4 view = cam->GetViewMatrix();
    Matrix4x4 proj = cam->GetPerspectiveFovMatrix();
    Matrix4x4 vp   = cam->GetViewportMatrix();
    Matrix4x4 vpv  = Math::Multiply(view, Math::Multiply(proj, vp));
    Matrix4x4 inv  = Math::Inverse(vpv);

    Vector3 p0 = Math::Transform(Vector3{ screen.x, screen.y, 0.0f }, inv);
    Vector3 p1 = Math::Transform(Vector3{ screen.x, screen.y, 1.0f }, inv);
    Vector3 dir = Math::Subtract(p1, p0);

    float denom = dir.z;
    if (std::fabs(denom) < 1e-6f) { return p0; }
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

void Bullet::Initialize(Vector2 pos, float sin, float cos, Camera* camera) {
    pos_ = pos;
    radius_ = { 15.0f, 15.0f };
    velocity_ = { sin * 10.0f, cos * 10.0f }; // 発射方向に速度を設定
    isActive_ = false;
    recoverTime_ = 60;

    camera_ = camera;

    sphereRegion_ = std::make_unique<SphereRegion>();
    // テクスチャ名は適宜差し替え
    sphereRegion_->Initialize(camera_, "resources/uvChecker.png", 16);
}

void Bullet::JudgeScreen() {
    area_ = (pos_.x > 250.0f);
}

void Bullet::SpeedCalculation() {
    if (pos_.x - radius_.x <= 0.0f || pos_.x + radius_.x >= 500.0f) {
        velocity_.x *= -1.0f;
    }
    if (pos_.y - radius_.y <= 0.0f) {
        velocity_.y = 0.0f;
    }
    velocity_.y += kGravity * deltaTime;
}

void Bullet::Recover() {
    if (recoverTime_ >= 0) {
        recoverTime_--;
        if (recoverTime_ < 0) { isActive_ = false; }
    }
}

void Bullet::Update() {
    if (!isActive_) return;

    JudgeScreen();
    SpeedCalculation();

    // 座標更新
    pos_.x += velocity_.x;
    pos_.y -= velocity_.y;

    // 壁へのめり込み予防
    if (pos_.x - radius_.x - velocity_.x <= 0.0f) { pos_.x = pos_.x + kPos; }
    if (pos_.x + radius_.x + velocity_.x >= 500.0f) { pos_.x = pos_.x - kPos; }
}

void Bullet::Draw() {
    if (!isActive_) return;

    // スクリーン座標 → Z=0 のワールド座標へ
    const float targetZ = 0.0f;
    Vector3 wc = ScreenToWorldOnZ(camera_, pos_, targetZ);
    float wr = ScreenRadiusToWorld(camera_, pos_, radius_.x, targetZ);

    // 単一インスタンスを積んで描画
    sphereRegion_->ClearInstances();
    sphereRegion_->AddInstance(wc, wr); // 等方スケールの球として追加
    sphereRegion_->Draw();
}