#pragma once

#include "../IScene.h"

#include <memory>

#include "3D/TriangleClass.h"
#include "2D/Sprite.h"
#include "3D/SphereClass.h"
#include "3D/ObjClass.h"
#include "3D/ParticleClass.h"
#include "3D/CylinderClass.h"
#include "3D/PointLightClass.h"
#include "3D/SpotLightClass.h"
#include "audio/Bgm.h"
#include "camera/Camera.h"
#include "camera/DebugCamera.h"
#include "math/Vector2.h"
#include "math/shape/LinePrimitive.h"
#include "CollisionResult.h"

#include "Player.h"
#include "EnemyManager.h"

//BGM
#include <xaudio2.h>

// 前方宣言

class IrufemiEngine;

class InputManager;


/// <summary>
/// ゲーム
/// </summary>
class GameScene : public IScene {

private: // メンバ関数(ゲーム部分)
	
    void Reflection();
    void BulletRecovery();

private:

    //反発係数
    const float kCOR = 0.80f;
    const float deltaTime = 1.0f / 60.0f;

    //敵の生成間隔の最小値、最大値
    const float kMinSpawnTime = 1.0f;
    const float kMaxSpawnTime = 5.0f;

private: // メンバ変数(ゲーム部分)

    std::unique_ptr<Player> player_;
    //エネミーマネージャー
    std::unique_ptr<EnemyManager> e_Manager_;

    //地面
    Segment2D ground[2];
    CollisionResult p_result_[2];
    CollisionResult b_result_[2];

    //コア
    Sphere circle_;
    int coreHp_;
    //弾とサークルの距離
    Vector2 vec_[10];
    float dis_[10];

    //===乱数生成器===///
    //生成エンジンの型
    using RNG_Engine = std::mt19937;
    //乱数エンジン
    RNG_Engine randomEngine_;
    //使う分布
    std::uniform_real_distribution<float> enemy_x_;

    //===敵のランダム生成===//
    //経過時間のカウント
    float ingameTimer_;
    //生成した乱数を入れる箱
    float time_;
    //乱数の分布(敵を生成する目標時間)
    std::uniform_real_distribution<float> spawnTime_;

    std::unique_ptr<Bgm> bgm = nullptr;

private: // メンバ変数(システム)

    // カメラ
    std::unique_ptr<Camera> camera_ = nullptr;

    // デバッグカメラ
    std::unique_ptr<DebugCamera> debugCamera_ = nullptr;

    std::unique_ptr<PointLightClass> pointLight_ = nullptr;

    std::unique_ptr<SpotLightClass> spotLight_ = nullptr;

    int loadTexture = false;

    bool debugMode = false;

    // ポインタ参照

    // エンジン
    IrufemiEngine* engine_ = nullptr;

public: // メンバ関数

    // デストラクタ
    ~GameScene() {}

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize(IrufemiEngine* engine) override;

    /// <summary>
    /// 更新
    /// </summary>
    void Update() override;

    /// <summary>
    /// 描画
    /// </summary>
    void Draw() override;
};