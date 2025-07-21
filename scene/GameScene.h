#pragma once

#include "IScene.h"
#include "../3D/Triangle.h"
#include "../2D/Sprite.h"
#include "../3D/Sphere.h"
#include "../3D/Obj.h"
#include "../camera/Camera.h"
#include "../camera/DebugCamera.h"

#include <memory>

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

    std::unique_ptr<Triangle>  triangle = nullptr;
    bool isActiveTriangle = false;

    std::unique_ptr<Sprite> sprite = nullptr;
    bool isActiveSprite = false;

    std::unique_ptr<Sphere> sphere = nullptr;
    bool isActiveSphere = true;

    std::unique_ptr<Obj> obj = nullptr;
    bool isActiveObj = false;

    // BGM
    IXAudio2SourceVoice* bgmVoice = nullptr;
    // BGMVolume
    float bgmVolume = 0.01f;
    // カテゴリの初期選択インデックス
    int selectedCat = 0;   
    // トラックの初期選択インデックス
    int selectedTrack = 0;   

    int loadTexture = false;

    bool debugMode = false;

    // ポインタ参照

    // エンジン
    IrufemiEngine* engine_ = nullptr;


public: // メンバ関数

    // デストラクタ
    ~GameScene() {
    }

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