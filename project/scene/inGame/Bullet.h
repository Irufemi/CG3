#pragma once

#include "math/Vector2.h"

class Bullet{
public:
	void Initialize (Vector2 pos, float sin, float cos);

	void Update ();

	void Draw ();

	Vector2  GetPos () { return pos_; }
	bool GetIsActive () { return isActive_; }
	void SetIsActive () { isActive_ = true; }

private:
	//position
	Vector2  pos_;
	Vector2  radius_;

	//物理演算用の変数
	Vector2  velocity_;
	bool isActive_;
};

