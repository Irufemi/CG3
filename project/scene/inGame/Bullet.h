#pragma once

#include "math/Vector2.h"
#include <memory>
#include "3D/SphereRegion.h"

class Camera;

class Bullet {
public:
	void Initialize(Vector2 pos, float sin, float cos, Camera* camera);

	void JudgeScreen();
	void SpeedCalculation();
	void Recover();
	void Update();
	void Collect();
	void Draw();

	//プレイヤー
	Vector2 GetPositon() { return pos_; }
	void SetPosition(Vector2 pos) { pos_ = pos; }
	Vector2 GetRadius() { return radius_; }
	Vector2 GetVelocity() { return velocity_; }
	void SetVelocity(Vector2 velocity) { velocity_ = velocity; }
	Vector2 GetReflect() { return reflect_; }
	void SetReflect(Vector2 velocity) { reflect_ = velocity; }
	bool GetIsActive() { return isActive_; }
	void SetIsActive(bool flag) { isActive_ = flag; }
	bool GetArea() { return area_; }
	bool GetWallTouch() { return wallTouch_; }
	void SetWallTouch() { wallTouch_ = false; }
	int GetRecoverTime() { return recoverTime_; }

	//回収完了を知らせる関数
	bool IsRecovered() { return recoverTime_ == 0 && isActive_ == true; }

private:
	// 描画用生成物
	std::unique_ptr<SphereRegion> sphereRegion_ = nullptr;

	Camera* camera_ = nullptr;

private:
	//position
	Vector2  pos_;
	Vector2  radius_;

	//物理演算用の変数
	Vector2 velocity_;

	bool isActive_;
	bool area_;
	Vector2 reflect_;
	//壁に触ったか
	bool wallTouch_;
	//回収タイマー
	int recoverTime_;

	static inline const float kGravity = -8.5f;
	static inline const float deltaTime = 1.0f / 60.0f;
	//めり込み防止用の定数
	static inline const float kPos = 3.0f;
};

