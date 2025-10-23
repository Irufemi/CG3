#pragma once
#include "Bullet.h"
#include <array>

#include "math/Vector2.h"
#include <memory>

class SphereClass;
class Camera;

class InputManager;

class Player{
public:
	void Initialize (InputManager* inputManager,Camera * camera);

	//固有の処理
	void Jump ();
	void Rotate ();
	void Fire ();
	void SpeedCalculation ();
	void Input ();
	void Update ();
	void Draw () ;
	void DrawSet();

	//プレイヤー
	Vector2 GetPositon () { return pos_; }
	void SetPosition (Vector2 pos) { pos_ = pos; }
	Vector2 GetRadius () { return radius_; }
	Vector2 GetVelocity () { return velocity_; }
	void SetVelocity (Vector2 velocity) { velocity_ = velocity; }

private:
	// 描画用生成物
	std::unique_ptr<SphereClass> sphere_ = nullptr;

private:
	//position
	Vector2  pos_;
	Vector2  radius_;

	//物理演算用の変数
	Vector2  velocity_;
	//砲台の変数
	Vector2  cannonPos_;
	Vector2  cannonRadius_;
	//回転用
	Vector2 cannonOffset_;
	float angle_;
	float rad_;
	float sinf_;
	float cosf_;
	Vector2 newPos_;


	static inline const float kGravity = -9.8f;
	static inline const float kMaxFallSpeed = 20.0f;
	static inline const float deltaTime = 1.0f / 60.0f;

	//弾
	std::array<Bullet, 10> bullet;

	// 外部参照

	//キーボード
	InputManager* inputManager_ = nullptr;
	Camera* camera_ = nullptr;
};

