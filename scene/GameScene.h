#pragma once

#include "IScene.h"

#include <memory>

#include "../3D/Triangle.h"
#include "../2D/Sprite.h"
#include "../3D/Sphere.h"
#include "../3D/Obj.h"
#include "../audio/Bgm.h"
#include "../camera/Camera.h"
#include "../camera/DebugCamera.h"

//BGM
#include <xaudio2.h>

// 前方宣言

class IrufemiEngine;


/// <summary>
/// ゲーム
/// </summary>
class GameScene : public IScene {
private: // メンバ変数

    // カメラ
    std::unique_ptr<Camera> camera = nullptr;

    // デバッグカメラ
    std::unique_ptr<DebugCamera> debugCamera = nullptr;

    std::unique_ptr<Obj> obj = nullptr;
    bool isActiveObj = false;

    std::unique_ptr<Sprite> sprite = nullptr;
    bool isActiveSprite = false;

    std::unique_ptr<Sphere> sphere = nullptr;
    bool isActiveSphere = true;

    std::unique_ptr<Obj> utashTeapot = nullptr;
    bool isActiveUtashTeapot = false;

    std::unique_ptr<Obj> stanfordBunny = nullptr;
    bool isActiveStanfordBunny = false;

    std::unique_ptr<Obj> multiMesh = nullptr;
    bool isActiveMultiMesh = false;

    std::unique_ptr<Obj> multiMaterial = nullptr;
    bool isActiveMultiMaterial = false;

    std::unique_ptr<Obj> suzanne = nullptr;
    bool isActiveSuzanne = false;
    
    std::unique_ptr<Bgm> bgm = nullptr;

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