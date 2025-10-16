#pragma once

#include "../IScene.h"

#include <memory>

#include "../../3D/TriangleClass.h"
#include "../../2D/Sprite.h"
#include "../../3D/SphereClass.h"
#include "../../3D/ObjClass.h"
#include "../../3D/ParticleClass.h"
#include "../../audio/Bgm.h"
#include "../../camera/Camera.h"
#include "../../camera/DebugCamera.h"

#include "application/Player.h"
#include "application/Blocks.h"
#include "application/MapChipField.h"

//BGM
#include <xaudio2.h>

// 前方宣言

class IrufemiEngine;

class InputManager;


/// <summary>
/// ゲーム
/// </summary>
class GameScene : public IScene {
private: // メンバ変数

    // カメラ
    std::unique_ptr<Camera> camera_ = nullptr;

    // デバッグカメラ
    std::unique_ptr<DebugCamera> debugCamera = nullptr;

    int loadTexture = false;

    bool debugMode = false;

    /// マップチップフィールド
    std::unique_ptr<MapChipField> mapChipField_ = nullptr;

    /// ブロック

    // ブロック群
    std::unique_ptr<class Blocks> blocks_ = nullptr;
    // ワールドトランスフォーム(ブロック)
    std::vector<std::vector<Transform*>> worldtransformBlocks_;

    /// 自キャラ

    // 自キャラ
    std::shared_ptr<Player> player_ = nullptr;
    // 3Dモデルデータ(自キャラ)
    std::unique_ptr<ObjClass> modelplayer_ = nullptr;

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

    void GenerateBlocks();
};