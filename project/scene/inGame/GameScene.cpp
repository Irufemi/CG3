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
    pointLight_->SetPos(Vector3{ 0.0f,30.0f,0.0f });

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

    e_Manager_ = std::make_unique<EnemyManager>();
    e_Manager_->Initialize(camera_.get()); 
    ground[0] = {
        {0.0f, 650.0f},
        {250.0f, 700.0f},
    };
    ground[1] = {
        {250.0f, 700.0f},
        {500.0f, 650.0f},
    };
    for (int i = 0; i < 2; i++) {
        p_result_[i] = {};
        b_result_[i] = {};
    }
    circle_ = {
        {250.0f, 700.0f},
        {30.0f},
    };
    coreHp_ = 3;
    for (int i = 0; i < 10; i++) {
        vec_[i] = {};
        dis_[i] = {};
    }

    //乱数生成機
    // 実行ごとに異なるシード値を取得するでやんす
    std::random_device rd;
    // std::mt19937 エンジンのインスタンスを作成し、rd()の結果で初期化するでやんす
    randomEngine_.seed(rd());
    //分布の初期化
    enemy_x_ = std::uniform_real_distribution<float>(25.0f, 475.0f);
    spawnTime_ = std::uniform_real_distribution<float>(kMinSpawnTime, kMaxSpawnTime);

    ingameTimer_ = 0.0f;
    time_ = spawnTime_(randomEngine_);
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

    player_->Input();
    player_->SpeedCalculation();
    player_->Update();
    BulletRecovery();

    Reflection();

    //時間のカウント
    ingameTimer_ += deltaTime;

    if (ingameTimer_ >= time_) {
        //敵を生成する	
        e_Manager_->Spawn(enemy_x_(randomEngine_), Vector2{ circle_.center.x,circle_.center.y });

        //経過時間から今回スポーンにかかった時間を減算
        ingameTimer_ -= time_;

        time_ = spawnTime_(randomEngine_);
    }

    //敵の更新処理
    for (auto& e : e_Manager_->GetEnemies()) {
        e.Update();

        //敵とコア
        if (e.IsCollision(Vector2{ circle_.center.x,circle_.center.y }, circle_.radius)) {
            if (e.GetIsAlive())
                coreHp_--;
            e.SetIsAlive();
        }

        //敵と弾
        for (auto& b : player_->GetBullet())
            if (e.IsCollision(b.GetPositon(), b.GetRadius().x)) {
                e.SetIsAlive();
            }

        //敵とプレイヤー
        if (e.IsCollision(player_->GetPositon(), player_->GetRadius().x)) {
            e.SetIsAlive();
        }
    }

    e_Manager_->EraseEnemy();

    ImGui::Text("coreHp %d", coreHp_);
    ImGui::Text("bulletNum : %d", player_->GetBulletNum());
    ImGui::Text("enemyNum : %d", e_Manager_->GetEnemies().size());

    e_Manager_->Update(deltaTime);

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

    // 回収部分
    // Shape::DrawEllipse(circle_.pos.x, circle_.pos.y, circle_.radius.x, circle_.radius.y, 0.0f, BLUE, kFillModeSolid);

    // Player
    player_->Draw();

    engine_->ApplyRegionPSO();
    player_->BulletDraw();

    // 個別 e.Draw() ループを削除し、まとめて描画
    e_Manager_->Draw(camera_.get());

    engine_->ApplyPSO();


    // 地面
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

void GameScene::Reflection() {
    //画面のどっちにいるかの一時フラグ
    bool area = false;

    if (player_->GetPositon().x > 250.0f) {
        area = true;
    } else {
        area = false;
    }

    for (int i = 0; i < 2; i++) {
        //プレイヤーと地面の当たり判定
        p_result_[i] = isCollision(player_->GetPositon(), player_->GetRadius(), ground[i]);
        if (p_result_[i].isColliding) {
            //めり込みを直す
            player_->SetPosition(player_->GetPositon() + (p_result_[i].penetration * p_result_[i].normal));
            //反射ベクトル更新
            player_->SetReflect(Reflect(player_->GetVelocity(), p_result_[i].normal));

            //反射ベクトルをxだけ反転させるように一時的に宣言
            Vector2 reflect = player_->GetReflect();

            if (!area) { // 左側
                if (player_->GetVelocity().x > 0.0f && player_->GetWallTouch()) {
                    reflect.x = std::abs(reflect.x);
                    reflect.x += 4.0f;
                } else if (player_->GetVelocity().x > 0.0f) {
                    reflect.x = std::abs(reflect.x);
                } else if (player_->GetVelocity().x < 0.0f) {
                    reflect.x *= -1.0f;
                }
            }
            if (area) { // 右側
                if (player_->GetVelocity().x < 0.0f && player_->GetWallTouch()) {
                    reflect.x = -std::abs(reflect.x);
                    reflect.x -= 4.0f;
                } else if (player_->GetVelocity().x < 0.0f) {
                    reflect.x = -std::abs(reflect.x);
                } else if (player_->GetVelocity().x > 0.0f) {
                    reflect.x *= -1.0f;
                }
            }
            player_->SetWallTouch();
            //プレイヤーの速度に掛ける
            player_->SetVelocity(reflect * kCOR);
        }

        //弾と地面の当たり判定
        for (auto& b : player_->GetBullet()) {
            b_result_[i] = isCollision(b.GetPositon(), b.GetRadius(), ground[i]);
            if (b_result_[i].isColliding) {
                //めり込みを直す
                b.SetPosition(b.GetPositon() + (b_result_[i].penetration * b_result_[i].normal));
                //反射ベクトル更新
                b.SetReflect(Reflect(b.GetVelocity(), b_result_[i].normal));

                //一時的な反射ベクトル
                Vector2 reflect = b.GetReflect();

                if (!b.GetArea()) { // 左側
                    if (b.GetVelocity().x > 0.0f && b.GetWallTouch()) {
                        reflect.x = std::abs(reflect.x);
                        reflect.x += 4.0f;
                    }
                    if (b.GetVelocity().x > 0.0f) {
                        reflect.x = std::abs(reflect.x);
                    }
                    if (b.GetVelocity().x < 0.0f) {
                        reflect.x *= -1.0f;
                    }
                }
                if (b.GetArea()) { // 右側
                    if (b.GetVelocity().x < 0.0f && b.GetWallTouch()) {
                        reflect.x = -std::abs(reflect.x);
                        reflect.x -= 4.0f;
                    }
                    if (b.GetVelocity().x < 0.0f) {
                        reflect.x = -std::abs(reflect.x);
                    }
                    if (b.GetVelocity().x > 0.0f) {
                        reflect.x *= -1.0f;
                    }
                }
                //弾の速度に掛ける
                b.SetVelocity(reflect * kCOR);
            }
        }
    }

}

void GameScene::BulletRecovery() {
    int i = 0;
    for (auto& b : player_->GetBullet()) {
        //サークルから弾の差分ベクトルを出して距離にする
        vec_[i] = { Vector2{circle_.center.x,circle_.center.y} - b.GetPositon() };
        dis_[i] = { Math::Length(vec_[i]) };

        //弾の速度が0且つサークルに当たってたら
        if (dis_[i] <= circle_.radius && b.GetVelocity().x <= 0.02f && b.GetVelocity().y <= 0.02f) {
            b.Recover();
        }
        i++;
    }
}