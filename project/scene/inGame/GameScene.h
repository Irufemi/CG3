#pragma once

#include "../IScene.h"

#include <memory>

#include "../../3D/TriangleClass.h"
#include "../../2D/Sprite.h"
#include "../../3D/SphereClass.h"
#include "../../3D/ObjClass.h"
#include "../../3D/ParticleClass.h"
#include "../../3D/PointLightClass.h"
#include "../../3D/SpotLightClass.h"
#include "../../audio/Bgm.h"
#include "../../camera/Camera.h"
#include "../../camera/DebugCamera.h"

//BGM
#include <xaudio2.h>

// 前方宣言

class IrufemiEngine;

class InputManager;


/// <summary>
/// ゲーム
/// </summary>
class GameScene : public IScene {
private: // 描画物その他

    bool isActiveObj_ = false;
    bool isActiveSprite_ = false;
    bool isActiveTriangle_ = false;
    bool isActiveSphere_ = true;
    bool isActiveStanfordBunny_ = false;
    bool isActiveUtashTeapot_ = false;
    bool isActiveMultiMesh_ = false;
    bool isActiveMultiMaterial_ = false;
    bool isActiveSuzanne_ = false;
    bool isActiveFence_ = false;
    bool isActiveTerrain_ = true;
    bool isActiveParticle_ = false;

    std::unique_ptr <Sprite> sprite = nullptr;

    std::unique_ptr <TriangleClass> triangle = nullptr;

    std::unique_ptr<ObjClass>  obj = nullptr;

    std::unique_ptr<SphereClass>  sphere = nullptr;

    std::unique_ptr<ObjClass>  stanfordBunny = nullptr;

    std::unique_ptr<ObjClass>  utashTeapot = nullptr;

    std::unique_ptr<ObjClass>  multiMesh = nullptr;

    std::unique_ptr<ObjClass>  multiMaterial = nullptr;

    std::unique_ptr<ObjClass>  suzanne = nullptr;

    std::unique_ptr<ObjClass>  fence_ = nullptr;

    std::unique_ptr<ObjClass>  terrain_ = nullptr;

    std::unique_ptr<ParticleClass>  particle = nullptr;

    std::unique_ptr<Bgm> bgm = nullptr;

private: // メンバ変数

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