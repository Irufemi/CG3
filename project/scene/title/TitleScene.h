#pragma once

#include "../IScene.h"

#include "audio/Bgm.h"
#include "audio/Se.h"
#include "3D/ObjClass.h"
#include "camera/Camera.h"
#include "camera/DebugCamera.h"
#include "3D/PointLightClass.h"
#include "3D/SpotLightClass.h"
#include <memory>

/// <summary>
/// タイトル
/// </summary>
class TitleScene : public IScene {

private:

    std::unique_ptr<Camera> camera_ = nullptr;

    // デバッグカメラ
    std::unique_ptr<DebugCamera> debugCamera_ = nullptr;

    std::unique_ptr<Se> se_select = nullptr;

    std::unique_ptr<Bgm> bgm_ = nullptr;

    std::unique_ptr<ObjClass> text_dan_1 = nullptr;
    std::unique_ptr<ObjClass> text_dan_2 = nullptr;
    std::unique_ptr<ObjClass> text_ri = nullptr;
    std::unique_ptr<ObjClass> text_sa = nullptr;
    std::unique_ptr<ObjClass> text_i = nullptr;
    std::unique_ptr<ObjClass> text_ku = nullptr;
    std::unique_ptr<ObjClass> text_ru = nullptr;

    std::unique_ptr<PointLightClass> pointLight_ = nullptr;

    std::unique_ptr<SpotLightClass> spotLight_ = nullptr;

    bool debugMode = false;

public: // メンバ関数

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

private: // メンバ変数
    IrufemiEngine* engine_ = nullptr;

};

