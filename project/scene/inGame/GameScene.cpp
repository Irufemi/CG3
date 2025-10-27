#define NOMINMAX
#include "GameScene.h"

#include "../SceneManager.h"
#include "../SceneName.h"
#include "engine/IrufemiEngine.h"
#include "imgui.h"

#include "InGameFunction.h"

#include <algorithm>
#include <cmath>

// --- Player と同等のスクリーン→ワールド変換ヘルパー ---
static Vector3 ScreenToWorldOnZ(const Camera* cam, const Vector2& screen, float targetZ) {
    Matrix4x4 view = cam->GetViewMatrix();
    Matrix4x4 proj = cam->GetPerspectiveFovMatrix();
    Matrix4x4 vp = cam->GetViewportMatrix();
    Matrix4x4 vpv = Math::Multiply(view, Math::Multiply(proj, vp));
    Matrix4x4 inv = Math::Inverse(vpv);

    Vector3 p0 = Math::Transform(Vector3{ screen.x, screen.y, 0.0f }, inv);
    Vector3 p1 = Math::Transform(Vector3{ screen.x, screen.y, 1.0f }, inv);
    Vector3 dir = Math::Subtract(p1, p0);

    float denom = dir.z;
    if (std::fabs(denom) < 1e-6f) {
        return p0;
    }
    float t = (targetZ - p0.z) / denom;
    return Math::Add(p0, Math::Multiply(t, dir));
}

// 画面半径[pixels]→Z=targetZ平面でのワールド半径
static float ScreenRadiusToWorld(const Camera* cam, const Vector2& center, float radiusPx, float targetZ) {
    Vector3 wc = ScreenToWorldOnZ(cam, center, targetZ);
    Vector3 wx = ScreenToWorldOnZ(cam, Vector2{ center.x + radiusPx, center.y }, targetZ);
    Vector2 d = Math::Subtract(Vector2{ wx.x, wx.y }, Vector2{ wc.x, wc.y });
    return Math::Length(d);
}

// 初期化
void GameScene::Initialize(IrufemiEngine* engine) {

    // 参照したものをコピー
    // エンジン
    this->engine_ = engine;

    camera_ = std::make_unique <Camera>();
    camera_->Initialize(engine_->GetClientWidth(), engine_->GetClientHeight());
    // Titleと同じカメラレイアウトに統一
    camera_->SetTranslate(Vector3{ 0.0f, 0.0f, -10.0f });
    camera_->UpdateMatrix();

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
    bgm->Initialize("resources/bgm/BGM_InGame.mp3");
    bgm->PlayFixed();

    player_ = std::make_unique<Player>();
    player_->Initialize(engine_->GetInputManager(), camera_.get());

    e_Manager_ = std::make_unique<EnemyManager>();
    e_Manager_->Initialize(camera_.get());

    gameOver_ = false;

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
        {90.0f},
    };
    coreHp_ = 3;
    for (int i = 0; i < 10; i++) {
        vec_[i] = {};
        dis_[i] = {};
    }

    // 地面用Cylinder生成
    for (int i = 0; i < 2; i++) {
        groundObj_[i] = std::make_unique<CylinderClass>();
        groundObj_[i]->Initialize(camera_.get(),"resources/whiteTexture.png");
    }

    // --- 床をスクリーン→ワールドに一度だけ変換して固定 ---
    // 内側の継ぎ目で小さく重ねるピクセル量（画面ピクセル単位）
    const float innerOverlapPx = 4.0f; // 見た目で調整してください（2〜8pxが目安）
    const float kPi = 3.14159265f;

    float radii[2] = { 0.0f, 0.0f };
    for (int i = 0; i < 2; i++) {
        Vector2 p0s = ground[i].origin;
        Vector2 p1s = ground[i].end;

        Vector2 d = Math::Subtract(p1s, p0s);
        float L = Math::Length(d);
        Vector2 dir = (L > 0.0f) ? Math::Multiply(1.0f / L, d) : Vector2{ 1.0f, 0.0f };

        // 左（i==0）は内側を前方へ少し伸ばす、右（i==1）は内側を後方へ少し伸ばす
        Vector2 p0v, p1v;
        if (i == 0) {
            p0v = p0s; // 外側はそのまま
            p1v = Math::Add(p1s, Math::Multiply(innerOverlapPx, dir)); // 内側を少し拡張
        } else {
            p0v = Math::Add(p0s, Math::Multiply(-innerOverlapPx, dir)); // 内側を少し前方へ移動（重ねる）
            p1v = p1s; // 外側はそのまま
        }

        Vector3 w0 = ScreenToWorldOnZ(camera_.get(), p0v, 0.0f);
        Vector3 w1 = ScreenToWorldOnZ(camera_.get(), p1v, 0.0f);

        groundWorld_[i].origin = Vector2{ w0.x, w0.y };
        groundWorld_[i].end = Vector2{ w1.x, w1.y };

        Vector3 center = Math::Multiply(0.5f, Math::Add(w0, w1));
        center.z += 0.1f; // わずかに手前へ
        Vector2 d2 = Vector2{ w1.x - w0.x, w1.y - w0.y };
        float length = Math::Length(d2);
        Vector2 centerPx = Math::Multiply(0.5f, Math::Add(p0v, p1v));
        float radius = ScreenRadiusToWorld(camera_.get(), centerPx, groundThicknessPx_ * 0.5f, 0.0f);

        groundObj_[i]->SetInfo(Cylinder{ center, radius, length });

        float theta = std::atan2(d2.y, d2.x);
        float zAngle = theta - (kPi * 0.5f);
        groundObj_[i]->SetRotate(Vector3{ 0.0f, 0.0f, zAngle });

        radii[i] = radius;
    }

    // 両側の半径の大きい方をシーン共通の床半径として使う（当たり判定用）
    groundRadiusWorld_ = std::max(radii[0], radii[1]);

    // 両シリンダーの Z 中心を揃えて段差を減らす
    {
        Vector3 c0 = groundObj_[0]->GetInfo().center;
        Vector3 c1 = groundObj_[1]->GetInfo().center;
        float commonZ = std::max(c0.z, c1.z);
        c0.z = c1.z = commonZ;
        groundObj_[0]->SetCenter(c0);
        groundObj_[1]->SetCenter(c1);
    }

    groundConverted_ = true;

    //乱数生成機
    // 実行ごとに異なるシード値を取得する
    std::random_device rd;
    // std::mt19937 エンジンのインスタンスを作成し、rd()の結果で初期化する
    randomEngine_.seed(rd());
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

    // ゲームシステムの更新
    GameSystem();

    // BGM
    bgm->Update();

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

    // 地面（3D）
    for (int i = 0; i < 2; i++) {
        if (groundObj_[i]) { groundObj_[i]->Draw(); }
    }

    // Player
    player_->Draw();


    //倍率ゾーン
    //DrawEllipse(circle_.center.x, circle_.center.y, 750.0f, 750.0f, 0.0f, 0xffff00ff, kFillModeSolid);
    //DrawEllipse(circle_.center.x, circle_.center.y, 583.2f, 583.2f, 0.0f, 0xbbbb00ff, kFillModeSolid);
    //DrawEllipse(circle_.center.x, circle_.center.y, 316.6f, 316.6f, 0.0f, 0x888800ff, kFillModeSolid);

    engine_->ApplyRegionPSO();
    player_->BulletDraw();

    // 個別 e.Draw() ループを削除し、まとめて描画
    e_Manager_->Draw(camera_.get());

    engine_->ApplyPSO();


    // Particle
    engine_->SetBlend(BlendMode::kBlendModeAdd);
    engine_->SetDepthWrite(PSOManager::DepthWrite::Disable);
    engine_->ApplyParticlePSO();

    // 2D
    engine_->SetBlend(BlendMode::kBlendModeNormal);
    engine_->SetDepthWrite(PSOManager::DepthWrite::Enable);
    engine_->ApplySpritePSO();
}

void GameScene::GameSystem() {
#if defined(_DEBUG) || defined(DEVELOPMENT)

    ImGui::Text("coreHp %d", coreHp_);
    ImGui::Text("bulletNum : %d", player_->GetBulletNum());
    ImGui::Text("enemyNum : %d", e_Manager_->GetEnemies().size());
    ImGui::Text("playerToCore :%f", player_->GetDisToCore());
#endif
    //時間のカウント
    ingameTimer_ += deltaTime;
    player_->Input();
    player_->SpeedCalculation();
    player_->disCalculation(Vector2{ circle_.center.x,circle_.center.y });
    player_->Update();
    BulletRecovery();

    Reflection();
    EnemyProcess();

    if (coreHp_ == 0) {
        gameOver_ = true;
    }

    // playerの座標などを描画物に反映
    player_->DrawSet();

    // 地面のワールド反映（カメラ更新後毎フレーム）
    UpdateGroundObjects();
}

void GameScene::EnemyProcess() {

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

                if (player_->GetDisToCore() <= 316.6f) {
                    player_->CollectBullet(1);
                } else if (player_->GetDisToCore() <= 583.2f) {
                    player_->CollectBullet(2);
                } else if (player_->GetDisToCore() <= 750.0f) {
                    player_->CollectBullet(3);
                }
            }

        //敵とプレイヤー
        if (e.IsCollision(player_->GetPositon(), player_->GetRadius().x)) {
            e.SetIsAlive();
            player_->SetIsStan();
        }
    }

    e_Manager_->EraseEnemy();
#if defined(_DEBUG) || defined(DEVELOPMENT)

    ImGui::Text("coreHp %d", coreHp_);
    ImGui::Text("bulletNum : %d", player_->GetBulletNum());
    ImGui::Text("enemyNum : %d", e_Manager_->GetEnemies().size());
#endif
    e_Manager_->Update(deltaTime);

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
                } else if (player_->GetVelocity().x >= 0.0f) {
                    reflect.x = std::abs(reflect.x);
                } else if (player_->GetVelocity().x <= 0.0f) {
                    reflect.x *= -1.0f;
                }
            }
            if (area) { // 右側
                if (player_->GetVelocity().x < 0.0f && player_->GetWallTouch()) {
                    reflect.x = -std::abs(reflect.x);
                    reflect.x -= 4.0f;
                } else if (player_->GetVelocity().x <= 0.0f) {
                    reflect.x = -std::abs(reflect.x);
                } else if (player_->GetVelocity().x >= 0.0f) {
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
                    } else if (b.GetVelocity().x >= 0.0f) {
                        reflect.x = std::abs(reflect.x);
                    } else if (b.GetVelocity().x <= 0.0f) {
                        reflect.x *= -1.0f;
                    }
                }
                if (b.GetArea()) { // 右側
                    if (b.GetVelocity().x < 0.0f && b.GetWallTouch()) {
                        reflect.x = -std::abs(reflect.x);
                        reflect.x -= 4.0f;
                    } else if (b.GetVelocity().x <= 0.0f) {
                        reflect.x = -std::abs(reflect.x);
                    } else if (b.GetVelocity().x >= 0.0f) {
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
        if (b.GetIsActive() && dis_[i] <= circle_.radius + b.GetRadius().y && b.GetVelocity().x <= 0.01f && b.GetVelocity().y <= 0.01f) {
            b.Recover();
        }

        i++;
    }
}

// 追加: 地面シリンダーをスクリーン→ワールドに反映
void GameScene::UpdateGroundObjects() {
    // 内側で重ねる px（画面ピクセル）
    const float innerOverlapPx = 4.0f;

    for (int i = 0; i < 2; i++) {
        if (!groundObj_[i]) continue;

        Vector2 p0s = ground[i].origin;
        Vector2 p1s = ground[i].end;

        Vector2 d = Math::Subtract(p1s, p0s);
        float L = Math::Length(d);
        Vector2 dir = (L > 0.0f) ? Math::Multiply(1.0f / L, d) : Vector2{ 1.0f, 0.0f };

        Vector2 p0v, p1v;
        if (i == 0) {
            p0v = p0s;
            p1v = Math::Add(p1s, Math::Multiply(innerOverlapPx, dir)); // 左床の内側を少し拡張
        } else {
            p0v = Math::Add(p0s, Math::Multiply(-innerOverlapPx, dir)); // 右床の内側を少し左へ寄せる
            p1v = p1s;
        }

        // スクリーン→Z=0ワールド
        Vector3 w0 = ScreenToWorldOnZ(camera_.get(), p0v, 0.0f);
        Vector3 w1 = ScreenToWorldOnZ(camera_.get(), p1v, 0.0f);

        // 中点・長さ・向き
        Vector3 center = Math::Multiply(0.5f, Math::Add(w0, w1));
        // keep a small front offset like Initialize
        center.z += 0.1f;
        Vector2 d2 = Vector2{ w1.x - w0.x, w1.y - w0.y };
        float   length = Math::Length(d2);

        // 太さ（ピクセル→ワールド半径） ... use center of the stretched segment in screen space
        Vector2 centerPx = Math::Multiply(0.5f, Math::Add(p0v, p1v));
        float radius = ScreenRadiusToWorld(camera_.get(), centerPx, groundThicknessPx_ * 0.5f, 0.0f);

        groundObj_[i]->SetInfo(Cylinder{ center, radius, length });

        // 向き: Cylinderの軸(Y+)を線分方向へ。zAngle = theta - 90deg
        float theta = std::atan2(d2.y, d2.x);
        float zAngle = theta - (3.14159265f * 0.5f);
        groundObj_[i]->SetRotate(Vector3{ 0.0f, 0.0f, zAngle });

        // Update debug name / UI
        groundObj_[i]->Update(i == 0 ? "Ground0" : "Ground1");
    }

    // 両側の半径を合わせ、Zを揃える（視覚差を抑える）
    if (groundObj_[0] && groundObj_[1]) {
        float r0 = groundObj_[0]->GetInfo().radius;
        float r1 = groundObj_[1]->GetInfo().radius;
        groundRadiusWorld_ = std::max(r0, r1);

        Vector3 c0 = groundObj_[0]->GetInfo().center;
        Vector3 c1 = groundObj_[1]->GetInfo().center;
        float commonZ = std::max(c0.z, c1.z);
        c0.z = c1.z = commonZ;
        groundObj_[0]->SetCenter(c0);
        groundObj_[1]->SetCenter(c1);
    }
}