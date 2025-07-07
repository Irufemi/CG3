#pragma once

#include "Vector3.h"
#include "Matrix4x4.h"
#include "Math.h"
#include "InputManager.h"
#include "Camera.h"

/*デバッグカメラ*/

/// <summary>
/// デバッグカメラ
/// </summary>
class DebugCamera {
private: //メンバ変数
    // カメラ注視点までの距離(ピボット回転)
    float distance_{};
    // 入力クラスのポインタ
    InputManager* input_ = nullptr;
    // スケーリング
    float scaleX_ = 1.0f;
    float scaleY_ = 1.0f;
    // カメラ
    Camera camera_{};
    // 回転行列
    Matrix4x4 matRot_{};

public: //メンバ関数
    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize(InputManager* input);

    /// <summary>
    /// 更新
    /// </summary>
    void Update();

    //ゲッター

    /// <summary>
    /// カメラを取得
    /// </summary>
    /// <returns>カメラ</returns>
    const Camera& GetCamera() { return camera_; }
};

