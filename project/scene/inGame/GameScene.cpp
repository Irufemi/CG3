#include "GameScene.h"

#include "../SceneManager.h"
#include "../SceneName.h"
#include "engine/IrufemiEngine.h"
#include "imgui.h"

#include "InGameFunction.h"

#include <algorithm>


// 初期化
void GameScene::Initialize(IrufemiEngine* engine) {

    // 参照したものをコピー
    // エンジン
    this->engine_ = engine;

    camera_ = std::make_unique <Camera>();
    camera_->Initialize(engine_->GetClientWidth(), engine_->GetClientHeight());

    debugCamera_ = std::make_unique <DebugCamera>();
    debugCamera_->Initialize(engine_->GetInputManager(), engine_->GetClientWidth(), engine_->GetClientHeight());
    debugMode = false;

    pointLight_ = std::make_unique <PointLightClass>();
    pointLight_->Initialize();
    pointLight_->SetPos(Vector3{ 0.0f,10.0f,0.0f });

    engine_->GetDrawManager()->SetPointLightClass(pointLight_.get());

    spotLight_ = std::make_unique <SpotLightClass>();
    spotLight_->Initialize();
    spotLight_->SetIntensity(0.0f);

    engine_->GetDrawManager()->SetSpotLightClass(spotLight_.get());

    bgm = std::make_unique<Bgm>();
    bgm->Initialize(engine_->GetAudioManager());
    bgm->PlayFirstTrack();

    player_ = std::make_unique<Player>();
    player_->Initialize(engine_->GetInputManager(),camera_.get());
    ground[0] = {
        {0.0f, 650.0f},
        {250.0f, 700.0f},
    };
    ground[1] = {
        {250.0f, 700.0f},
        {500.0f, 650.0f},
    };
    collisionResult_[0] = {};
    collisionResult_[1] = {};

    cylinder_ = std::make_unique<CylinderClass>();
    cylinder_->Initialize(camera_.get());
}

// 更新
void GameScene::Update() {

#if defined(_DEBUG) || defined(DEVELOPMENT)

    ImGui::Begin("GameScene");
    // pointLight 
    pointLight_->Debug();
    // spotLight 
    spotLight_->Debug();

    ImGui::End();

    ImGui::Begin("Texture");
    if (ImGui::Button("allLoadActivate")) {
        engine_->GetTextureManager()->LoadAllFromFolder("resources/");
    }
    ImGui::Checkbox("debugMode", &debugMode);
    ImGui::End();

#endif // _DEBUG

    // カメラの更新
    if (debugMode) {
        debugCamera_->Update();
        camera_->SetViewMatrix(debugCamera_->GetCamera().GetViewMatrix());
        camera_->SetPerspectiveFovMatrix(debugCamera_->GetCamera().GetPerspectiveFovMatrix());
    } else {
        camera_->Update("Camera");

    }

    cylinder_->Update();

    player_->Input();
    player_->SpeedCalculation();
    player_->Update();

    //画面のどっちにいるかの一時フラグ
    bool area = false;

    if (player_->GetPositon().x > 250.0f) {
        area = true;
    } else {
        area = false;
    }

    for (int i = 0; i < 2; i++) {
        //当たり判定
        collisionResult_[i] = isCollision(player_->GetPositon(), player_->GetRadius(), ground[i]);
        if (collisionResult_[i].isColliding) {
            //めり込みを直す
            player_->SetPosition(player_->GetPositon() + (collisionResult_[i].penetration * collisionResult_[i].normal));
            //反射させる
            reflect = Reflect(player_->GetVelocity(), collisionResult_[i].normal);

            //boundWall[0] = false;
            //boundWall[1] = false;

            //if (!xRef[i] ) {
            //	//x成分だけ符号を反転させる！
            //	reflect.x = -reflect.x;
            //	xRef[i] = true;
            //}

            if (!area) { // 左側
                if (player_->GetVelocity().x > 0.0f) {
                    reflect.x = std::abs(reflect.x);
                }
                if (player_->GetVelocity().x < 0.0f) {
                    reflect.x *= -1.0f;
                }
            }
            if (area) { // 右側
                if (player_->GetVelocity().x < 0.0f) {
                    reflect.x = -std::abs(reflect.x);
                }
                if (player_->GetVelocity().x > 0.0f) {
                    reflect.x *= -1.0f;
                }
            }

            //プレイヤーの速度に掛ける
            player_->SetVelocity(reflect * kCOR);
        }

#if defined(_DEBUG) || defined(DEVELOPMENT)

        ImGui::Text("normal x:%f y:%f", collisionResult_[i].normal.x, collisionResult_[i].normal.y);
        ImGui::Text("xRef %d", xRef[i]);
        ImGui::Text("boundWall %d", boundWall[i]);

#endif // DEBUG_
    }
#if defined(_DEBUG) || defined(DEVELOPMENT)

    ImGui::Text("velocity x:%f y:%f", player_->GetVelocity().x, player_->GetVelocity().y);
    ImGui::Text("reflect x:%f y:%f", reflect.x, reflect.y);

#endif // DEBUG_

    // BGM
    bgm->Update();

    // playerの座標などを描画物に反映
    player_->DrawSet();

    //キーが押されていたら
    if (PressedVK('P')) {
        if (g_SceneManager) {
            g_SceneManager->Request(SceneName::result);
        }
    }

}

// 描画
void GameScene::Draw() {

    // 3D

    engine_->SetBlend(BlendMode::kBlendModeNormal);
    engine_->SetDepthWrite(PSOManager::DepthWrite::Enable);
    engine_->ApplyPSO();

    cylinder_->Draw();

    // Player
    player_->Draw();

    //地面
    //Shape::DrawLine(ground[0].origin.x, ground[0].origin.y, ground[0].diff.x, ground[0].diff.y, BLACK);
    //Shape::DrawLine(ground[1].origin.x, ground[1].origin.y, ground[1].diff.x, ground[1].diff.y, BLACK);

    // Particle

    engine_->SetBlend(BlendMode::kBlendModeAdd);
    engine_->SetDepthWrite(PSOManager::DepthWrite::Disable);
    engine_->ApplyParticlePSO();

    // 2D

    engine_->SetBlend(BlendMode::kBlendModeNormal);
    engine_->SetDepthWrite(PSOManager::DepthWrite::Enable);
    engine_->ApplySpritePSO();

}
