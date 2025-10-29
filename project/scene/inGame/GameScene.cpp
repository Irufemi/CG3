#define NOMINMAX
#include "GameScene.h"

#include "../SceneManager.h"
#include "../SceneName.h"
#include "engine/IrufemiEngine.h"
#include "imgui.h"

#include "InGameFunction.h"

#include <algorithm>
#include <cmath>

// --- 0..1 のスムーズ補間（Hermite SmoothStep）---
static float Smooth01(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

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
    pointLight_->SetPos(Vector3{ 0.0f,-5.0f,0.0f });

    engine_->GetDrawManager()->SetPointLightClass(pointLight_.get());

    spotLight_ = std::make_unique <SpotLightClass>();
    spotLight_->Initialize();
    spotLight_->SetIntensity(0.0f);

    engine_->GetDrawManager()->SetSpotLightClass(spotLight_.get());

    se_enemy = std::make_unique<Se>();
    se_enemy->Initialize("resources/se/SE_Enemy.mp3");

    se_playerdamage = std::make_unique<Se>();
    se_playerdamage->Initialize("resources/se/SE_PlayerDamage.mp3");

    se_playerstan = std::make_unique<Se>();
    se_playerstan->Initialize("resources/se/sword1.mp3");

    se_playertoutch = std::make_unique<Se>();
    se_playertoutch->Initialize("resources/se/by_chance.mp3");

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
        groundObj_[i]->Initialize(camera_.get(), "resources/whiteTexture.png");
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

        // 左（i==0）は内側を前方へ少し伸ばす、右（i==1）は内側を後方へ少し移動
        Vector2 p0v, p1v;
        if (i == 0) {
            p0v = p0s; // 外側はそのまま
            p1v = Math::Add(p1s, Math::Multiply(innerOverlapPx, dir)); // 内側を少し拡張
        } else {
            p0v = Math::Add(p0s, Math::Multiply(-innerOverlapPx, dir)); // 内側を少し後方へ移動（重ねる）
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
    spawnTime_[0] = std::uniform_real_distribution<float>(kMinSpawnTimeFase1, kMaxSpawnTimeFase1);
    spawnTime_[1] = std::uniform_real_distribution<float>(kMinSpawnTimeFase2, kMaxSpawnTimeFase2);
    spawnTime_[2] = std::uniform_real_distribution<float>(kMinSpawnTimeFase3, kMaxSpawnTimeFase3);

    ingameTimer_ = 0.0f;
    Timer_ = 0.0f;
    time_ = spawnTime_[0](randomEngine_);

    // 背景倍率ゾーン（Circle2D）
    {
        const Vector3 centerWS{ circle_.center.x, circle_.center.y, 0.0f };
        const float radii[3] = { 750.0f, 583.2f, 316.6f };

        for (int i = 0; i < 3; ++i) {
            zoneCircles_[i] = std::make_unique<Circle2D>();
            zoneCircles_[i]->Initialize(camera_.get(), "");
            zoneCircles_[i]->SetUseTexture(false);
            zoneCircles_[i]->SetInfo({ centerWS, radii[i] });
        }
        // 外側は暗め（通常ブレンドで背景のみを薄く暗くする）
        zoneCircles_[0]->SetColor(Vector4{ 0.0f, 0.0f, 0.0f, 0.15f });
        // 中間は少し明るく（加算で黄を薄く）
        zoneCircles_[1]->SetColor(Vector4{ 1.0f, 0.95f, 0.4f, 0.20f });
        // 内側はさらに明るく（加算で強め）
        zoneCircles_[2]->SetColor(Vector4{ 1.0f, 0.95f, 0.4f, 0.35f });
    }

    text_period_ = std::make_unique<Sprite>();
    text_period_->Initialize(camera_.get(), "resources/texture/gameText_period.png");
    text_period_->SetAnchor(0.5f, 0.5f);
    text_period_->SetPosition(camera_->GetViewportWidth() / 2.0f, 32.0f);

    text_addEnemy_ = std::make_unique<Sprite>();
    text_addEnemy_->Initialize(camera_.get(), "resources/texture/gameText_addEnemy.png");
    text_addEnemy_->SetAnchor(0.5f, 0.5f);
    text_addEnemy_->SetPosition(camera_->GetViewportWidth() / 2.0f, camera_->GetViewportHeight() / 2.0f + 100.0f);

    text_pleaseAlive_ = std::make_unique<Sprite>();
    text_pleaseAlive_->Initialize(camera_.get(), "resources/texture/gametext_PlaeaseAlive.png");
    text_pleaseAlive_->SetAnchor(0.5f, 0.5f);
    text_pleaseAlive_->SetPosition(camera_->GetViewportWidth() / 2.0f, camera_->GetViewportHeight() / 2.0f - 100.0f);

    text_bullet_ = std::make_unique<Sprite>();
    text_bullet_->Initialize(camera_.get(), "resources/texture/gameText_bullet.png");
    text_bullet_->SetSize(96.0f, 38.0f);
    text_bullet_->SetPosition(300.0f, 720.0f);

    text_HP_ = std::make_unique<Sprite>();
    text_HP_->Initialize(camera_.get(), "resources/texture/gameText_HP.png");
    text_HP_->SetSize(48.0f, 38.0f);
    text_HP_->SetPosition(10.0f, 720.0f);

    text_slash_ = std::make_unique<Sprite>();
    text_slash_->Initialize(camera_.get(), "resources/texture/gameText_slash.png");
    text_slash_->SetSize(19.0f, 38.0f);
    text_slash_->SetPosition(438.0f, 720.0f);

    // ==== 弾数UI ====
    bulletNowText_ = std::make_unique<NumberText>();
    bulletNowText_->Initialize(camera_.get(), "resources/text_num.png", 32.0f, 64.0f, 2);
    bulletNowText_->SetTracking(0.0f);
    bulletNowText_->SetScale(0.6f);

    bulletMaxText_ = std::make_unique<NumberText>();
    bulletMaxText_->Initialize(camera_.get(), "resources/text_num.png", 32.0f, 64.0f, 2);
    bulletMaxText_->SetTracking(0.0f);
    bulletMaxText_->SetScale(0.6f);

    // --- 背景倍率ゾーン（Circle2D）既存ブロックの直後に追加 ---
    // 中心の回収円（Circle2D） — 赤色・塗りつぶし
    coreCircle_ = std::make_unique<Circle2D>();
    coreCircle_->Initialize(camera_.get(), ""); // テクスチャなしで描く
    coreCircle_->SetUseTexture(false);
    coreCircle_->SetInfo({ Vector3{ circle_.center.x, circle_.center.y, 0.0f }, circle_.radius });
    // 赤（アルファは見た目で調整してください）
    coreCircle_->SetColor(Vector4{ 1.0f, 0.0f, 0.0f, 0.35f });

    // --- ゲームオーバー拡大用パネル生成 ---
    {
        gameOverPlate_ = std::make_unique<Sprite>();
        gameOverPlate_->Initialize(camera_.get(), "resources/whiteTexture.png");
        // 画面中央に配置（中央アンカー）
        const float vw = camera_->GetViewportWidth();
        const float vh = camera_->GetViewportHeight();
        gameOverPlate_->SetAnchor(0.5f, 0.5f);
        gameOverPlate_->SetPosition(vw * 0.5f, vh * 0.5f, 0.0f);
        // 初期サイズはゼロ（中央から拡大する）
        gameOverPlate_->SetSize(0.0f, 0.0f);
        // 黒板で少し透過（必要に応じて変更）
        gameOverPlate_->SetColor(Vector4{ 0.0f, 0.0f, 0.0f, 0.85f });
        // 目標サイズ（画面の9割）
        gameOverTargetSize_ = Vector2{ vw * 0.9f, vh * 0.9f };
    }

    // ==== カウントダウン初期化 ====
    {
        const float vw = camera_->GetViewportWidth();
        const float vh = camera_->GetViewportHeight();

        countdownText_ = std::make_unique<NumberText>();
        // 0..9画像が横一列（32x64セル）を想定。表示桁は最初1桁
        countdownText_->Initialize(camera_.get(), "resources/text_num.png", 32.0f, 64.0f, 1);
        countdownText_->SetScale(1.0f);            // デフォルト大きく表示（ImGuiで調整可）
        countdownText_->SetTracking(0.0f);
        // 中心位置を画面中央へ
        countdownCenter_ = Vector2{ vw * 0.5f, vh * 0.5f };
        // カウント開始（初期値は countdownStartSeconds_ = 3）
        countdownTime_ = static_cast<float>(countdownStartSeconds_);
        countdownActive_ = true;
        countdownDisplayDigits_ = 1;
    }

    {
        const float vw = camera_->GetViewportWidth();

        gameTimerText_ = std::make_unique<NumberText>();
        // 0..9 が横一列のフォント画像を使い、桁数は最初4桁 (XX.XX)
        gameTimerText_->Initialize(camera_.get(), "resources/text_num.png", 32.0f, 64.0f, static_cast<size_t>(gameTimerDigits_));
        gameTimerText_->SetTracking(0.0f);
        gameTimerText_->SetScale(gameTimerScale_);
        // 初期表示位置は画面上部中央（ImGuiで調整可）
        gameTimerCenter_ = Vector2{ vw * 0.5f, 32.0f };
    }

    // === Initialize 内（coreCircle_ の直後に挿入） ===
    // HPアイコン初期化（SphereClass で描画）
    for (int i = 0; i < kMaxHpIcons; ++i) {
        hpIcons_[i] = std::make_unique<SphereClass>();
        hpIcons_[i]->Initialize(camera_.get(), "resources/whiteTexture.png"); // テクスチャ無し（必要なら指定）
        // スクリーン中心をワールドへ変換
        Vector2 screenCenter{ hpIconsPos_.x + i * (hpIconScreenRadius_ * 2.0f + hpIconSpacing_), hpIconsPos_.y };
        Vector3 worldCenter = ScreenToWorldOnZ(camera_.get(), screenCenter, hpIconTargetZ_);
        worldCenter.z += 0.05f; // わずかに手前へ（必要に応じ調整）
        float worldRadius = ScreenRadiusToWorld(camera_.get(), screenCenter, hpIconScreenRadius_, hpIconTargetZ_);
        Sphere s; s.center = worldCenter; s.radius = worldRadius;
        hpIcons_[i]->SetInfo(s);
        hpIcons_[i]->SetColor(hpIconColor_);
        hpIcons_[i]->Update(false);
    }
}

// 更新
void GameScene::Update() {

#if defined(_DEBUG) || defined(DEVELOPMENT)

    ImGui::Begin("GameScene");
    // pointLight / spotLight のデバッグ表示（既存）
    pointLight_->Debug();
    spotLight_->Debug();

    // HPアイコン調整パネル
    ImGui::SeparatorText("HP Icons");
    ImGui::Checkbox("Show HP Icons", &hpIconsVisible_);
    ImGui::DragFloat2("HP Icons Pos (px)", &hpIconsPos_.x, 1.0f, 0.0f, camera_->GetViewportWidth());
    ImGui::DragFloat("HP Icon Spacing (px)", &hpIconSpacing_, 1.0f, 0.0f, 300.0f);
    ImGui::DragFloat("HP Icon Screen Radius (px)", &hpIconScreenRadius_, 0.5f, 1.0f, 200.0f);
    ImGui::DragFloat("HP Icon Target Z", &hpIconTargetZ_, 0.01f, -50.0f, 50.0f);
    float col[4] = { hpIconColor_.x, hpIconColor_.y, hpIconColor_.z, hpIconColor_.w };
    if (ImGui::ColorEdit4("HP Icon Color", col)) {
        hpIconColor_ = Vector4{ col[0], col[1], col[2], col[3] };
    }

    ImGui::End();
#endif // _DEBUG

    // カメラの更新（既存処理）
    if (debugMode) {
        debugCamera_->Update();
        camera_->SetViewMatrix(debugCamera_->GetCamera().GetViewMatrix());
        camera_->SetPerspectiveFovMatrix(debugCamera_->GetCamera().GetPerspectiveFovMatrix());
    } else {
        camera_->Update("Camera");
    }

    // カウントダウン処理: カウント中は GameSystem をスキップ
    if (countdownActive_) {
        // deltaTime はファイル内の static 定義(1/60)を利用
        countdownTime_ -= deltaTime;
        if (countdownTime_ <= 0.0f) {
            countdownActive_ = false;
            countdownTime_ = 0.0f;
            // カウント終了時の追加処理があればここへ（例: 効果音）
        }
    } else {
        // 通常のゲームアップデートを実行
        GameSystem();
    }

    // BGM は常時更新しておく
    bgm->Update();

    // 背景倍率ゾーンの中心を同期（必要なら）
    if (zoneCircles_[0]) {
        const Vector3 centerWS{ circle_.center.x, circle_.center.y, 0.0f };
        for (int i = 0; i < 3; ++i) {
            zoneCircles_[i]->SetCenter(centerWS);
            zoneCircles_[i]->Update(i == 0 ? "ZoneOuter" : (i == 1 ? "ZoneMid" : "ZoneInner"));
        }
    }

    text_bullet_->Update(false);
    text_HP_->Update(false);
    text_slash_->Update(false);
    text_pleaseAlive_->Update(false);
    text_addEnemy_->Update(false);
    text_period_->Update(true,"text_period_");

#if defined(_DEBUG) || defined(DEVELOPMENT)
    // デバッグ UI

    ImGui::Begin("GameScene");


    ImGui::SeparatorText("Game Timer");
    ImGui::Checkbox("Show Timer", &showGameTimer_);
    float tcenter[2] = { gameTimerCenter_.x, gameTimerCenter_.y };
    if (ImGui::DragFloat2("Timer Center", tcenter, 1.0f)) {
        gameTimerCenter_.x = tcenter[0];
        gameTimerCenter_.y = tcenter[1];
    }
    if (ImGui::DragFloat("Timer Scale", &gameTimerScale_, 0.01f, 0.1f, 10.0f)) {
        if (gameTimerText_) gameTimerText_->SetScale(gameTimerScale_);
    }
    int digits = gameTimerDigits_;
    if (ImGui::SliderInt("Timer Digits (incl decimals)", &digits, 2, 6)) {
        gameTimerDigits_ = digits;
        if (gameTimerText_) gameTimerText_->SetMaxDigits(static_cast<size_t>(gameTimerDigits_));
    }
    ImGui::Text("Remaining = %.2f (Timer = %.2f)", std::max(0.0f, gameTimeLimitSec_ - Timer_), Timer_);


    ImGui::End();
#endif // _DEBUG

    // playerの座標などを描画物に反映
    player_->DrawSet();

    // 地面のワールド反映（カメラ更新後毎フレーム）
    UpdateGroundObjects();

    //キーが押されていたら
    if (PressedVK('P')) {
        if (g_SceneManager) {
            g_SceneManager->Request(SceneName::title);
        }
    }

    // --- GameOver 拡大演出の開始トリガ ---
    if (gameOver_ && !gameOverAnimPlayed_) {
        gameOverAnimActive_ = true;
        gameOverAnimPlayed_ = true; // 二度目以降は入らない

        gameOverAnimTime_ = 0.0f;

        // ウィンドウサイズが変わっている可能性に備えて取り直し
        const float vw = camera_->GetViewportWidth();
        const float vh = camera_->GetViewportHeight();
        gameOverTargetSize_ = Vector2{ vw * 0.9f, vh * 0.9f };
        if (gameOverPlate_) {
            gameOverPlate_->SetAnchor(0.5f, 0.5f);
            gameOverPlate_->SetPosition(vw * 0.5f, vh * 0.5f, 0.0f);
            gameOverPlate_->SetSize(0.0f, 0.0f);
        }
    }

    // --- GameOver 拡大アニメーション ---
    if (gameOverAnimActive_ && gameOverPlate_) {
        gameOverAnimTime_ += deltaTime;
        const float t = Smooth01(gameOverAnimTime_ / gameOverAnimDuration_);
        const float cw = gameOverTargetSize_.x * t;
        const float ch = gameOverTargetSize_.y * t;
        gameOverPlate_->SetSize(cw, ch);
        gameOverPlate_->Update(false, "GameOverPlate");

        if (gameOverAnimTime_ >= gameOverAnimDuration_) {
            // 最終サイズで確定
            gameOverPlate_->SetSize(gameOverTargetSize_.x, gameOverTargetSize_.y);
            // アニメーション終了。以降は再トリガーしない（Playedがtrueのため）
            gameOverAnimActive_ = false;
        }
    }
}

// 描画
void GameScene::Draw() {

    // --- 背面に倍率ゾーン（2D）---
    // 先に背景として描くことで他オブジェクトに影響しない
    engine_->SetDepthWrite(PSOManager::DepthWrite::Disable);
    engine_->ApplySpritePSO();

    // 外側：通常ブレンドで薄く暗く（背景のみ見える）
    engine_->SetBlend(BlendMode::kBlendModeNormal);
    if (zoneCircles_[0]) zoneCircles_[0]->Draw();

    // 中央側：加算ブレンドで明るさを足す
    engine_->SetBlend(BlendMode::kBlendModeAdd);
    if (zoneCircles_[1]) zoneCircles_[1]->Draw();
    if (zoneCircles_[2]) zoneCircles_[2]->Draw();

    // 回収部分（Circle2Dで描画）
    // （この直前で SpritePSO が適用され DepthWrite は Disable の状態）
    engine_->SetBlend(BlendMode::kBlendModeAdd); // 加算で中央を明るくする
    if (coreCircle_) coreCircle_->Draw();
    //回収部分
    /*Shape::DrawEllipse(circle_.pos.x, circle_.pos.y, circle_.radius.x, circle_.radius.y, 0.0f,
        BLUE, kFillModeSolid);*/

        // --- 以降は既存の3Dなど ---

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

    engine_->ApplyRegionPSO();
    player_->BulletDraw();

    // 敵
    e_Manager_->Draw(camera_.get());

    engine_->ApplyPSO();


    // Particle
    engine_->SetBlend(BlendMode::kBlendModeAdd);
    engine_->SetDepthWrite(PSOManager::DepthWrite::Disable);
    engine_->ApplyParticlePSO();

    // 2D（他のスプライトを描く場合のデフォルトへ戻す）
    engine_->SetBlend(BlendMode::kBlendModeNormal);
    engine_->SetDepthWrite(PSOManager::DepthWrite::Enable);
    engine_->ApplySpritePSO();

    // ==== 弾数UI描画 ====
    if (bulletNowText_ && bulletMaxText_) {
        const size_t nowSlotDigits = 2; // 0～10想定で2桁スロット
        const size_t maxDigits = 2; // 「10」

        // 現在弾数ブロックの右端座標（左上から固定幅で算出）
        const float nowRight = bulletUiLeftTop_.x + bulletNowText_->GetWidthForDigits(nowSlotDigits);
        bulletNowText_->SetPosRightTop(Vector2{ nowRight, bulletUiLeftTop_.y });

        // 「/」分の余白 + ブロック間余白を空けて最大値ブロックを配置
        const float maxLeft = nowRight + bulletUiSlashGap_ + bulletUiBlockGap_;
        const float maxRight = maxLeft + bulletMaxText_->GetWidthForDigits(maxDigits);
        bulletMaxText_->SetPosRightTop(Vector2{ maxRight, bulletUiLeftTop_.y });

        // 値の描画（スラッシュは後で追加予定）
        bulletNowText_->DrawNumber(static_cast<uint64_t>(player_->GetBulletNum()));
        bulletMaxText_->DrawString("10");
    }


    text_bullet_->Draw();
    text_HP_->Draw();
    text_slash_->Draw();
    text_period_->Draw();

    // === Draw 内：UI 描画エリア（text_bullet_ 等を描く前）へ追加 ===
    // HPアイコン描画（表示数は coreHp_）
    if (hpIconsVisible_) {
        const int drawCount = std::clamp(coreHp_, 0, kMaxHpIcons);
        if (drawCount > 0) {
            // 深度バッファを汚さないため DepthWrite を無効化して描画（Z位置での前寄せも併用）
            engine_->SetBlend(BlendMode::kBlendModeNormal);
            engine_->SetDepthWrite(PSOManager::DepthWrite::Disable);
            engine_->ApplyPSO();
            for (int i = 0; i < drawCount; ++i) {
                if (hpIcons_[i]) hpIcons_[i]->Draw();
            }
            // 後続の描画のため DepthWrite を戻す
            engine_->SetDepthWrite(PSOManager::DepthWrite::Enable);
            engine_->ApplyPSO();
        }
    }

    if (showGameTimer_ && gameTimerText_) {
        // 最前面の 2D 表示状態を確実にする
        engine_->SetBlend(BlendMode::kBlendModeNormal);
        engine_->SetDepthWrite(PSOManager::DepthWrite::Disable);
        engine_->ApplySpritePSO();

        // 残り時間（秒）を計算し、小数点以下2桁（センティ秒）に切り捨て
        const float remaining = std::max(0.0f, gameTimeLimitSec_ - Timer_);
        const int centis = static_cast<int>(std::floor(remaining * 100.0f + 1e-6f)); // 例: 59.987 -> 5998

        // 固定桁（例: 4桁 "XXYY" = "XX.YY" 相当）。必要なら gameTimerDigits_ を使って調整。
        const int pad = std::max(1, gameTimerDigits_);
        char fmt[8];
        sprintf_s(fmt, "%%0%dd", pad);
        char buf[16];
        sprintf_s(buf, fmt, centis); // 先頭0埋めで文字列生成（"5998", "0099", "6000" など）

        // 中央基準に合わせて rightTop を算出して配置
        const float totalW = gameTimerText_->GetWidthForDigits(static_cast<size_t>(pad));
        const float scaledH = gameTimerText_->GetCellH() * gameTimerText_->GetScale();
        const float rightX = gameTimerCenter_.x + totalW * 0.5f;
        const float topY = gameTimerCenter_.y - (scaledH * 0.5f);
        gameTimerText_->SetPosRightTop(Vector2{ rightX, topY });

        // ドット '.' は別スプライトで後から描く前提なので、ここでは数字のみ描画
        gameTimerText_->DrawString(std::string(buf));
    }

    // --- 最前面にゲームオーバーパネルを描画 ---
    if (gameOver_ && gameOverPlate_) {
        // 最前面に載せるため、DepthWriteを無効化してSprite PSOで描く
        engine_->SetBlend(BlendMode::kBlendModeNormal);
        engine_->SetDepthWrite(PSOManager::DepthWrite::Disable);
        engine_->ApplySpritePSO();
        gameOverPlate_->Draw();
    }

    // ==== カウントダウン描画（最前面・SpritePSO が適用された状態で） ====
    if (countdownActive_ && countdownText_) {
        text_pleaseAlive_->Draw();
        text_addEnemy_->Draw();

        engine_->SetBlend(BlendMode::kBlendModeNormal);
        engine_->SetDepthWrite(PSOManager::DepthWrite::Disable);
        engine_->ApplySpritePSO();

        const int disp = std::max(0, static_cast<int>(std::ceil(countdownTime_)));
        const size_t neededDigits = (disp >= 10) ? 2 : 1;
        if (static_cast<int>(neededDigits) != countdownDisplayDigits_) {
            countdownText_->SetMaxDigits(neededDigits);
            countdownDisplayDigits_ = static_cast<int>(neededDigits);
        }

        const float totalW = countdownText_->GetWidthForDigits(neededDigits);
        const float scaledH = countdownText_->GetCellH() * countdownText_->GetScale();
        const float rightX = countdownCenter_.x + totalW * 0.5f;
        const float topY = countdownCenter_.y - (scaledH * 0.5f);
        countdownText_->SetPosRightTop(Vector2{ rightX, topY });

        countdownText_->DrawNumber(static_cast<uint64_t>(disp));
    }
}

void GameScene::GameSystem() {
    //時間のカウント
    ingameTimer_ += deltaTime;
    Timer_ += deltaTime;
    player_->Input();
    player_->SpeedCalculation();
    player_->disCalculation(Vector2{ circle_.center.x,circle_.center.y });
    player_->Update();
    Reflection();
    BulletRecovery();
    EnemyProcess();

    if (coreHp_ == 0) {
        gameOver_ = true;
    }

#if defined(_DEBUG) || defined(DEVELOPMENT)

    ImGui::Text("coreHp %d", coreHp_);
    ImGui::Text("bulletNum : %d", player_->GetBulletNum());
    ImGui::Text("enemyNum : %d", e_Manager_->GetEnemies().size());
    ImGui::Text("playerToCore :%f", player_->GetDisToCore());
    ImGui::Text("Timer : %f", Timer_);
#endif
}

void GameScene::EnemyProcess() {

    if (ingameTimer_ >= time_) {
        //敵を生成する	
        e_Manager_->Spawn(enemy_x_(randomEngine_), Vector2{ circle_.center.x,circle_.center.y });

        //経過時間から今回スポーンにかかった時間を減算
        ingameTimer_ -= time_;

        if (Timer_ <= 1200.0f) {
            time_ = spawnTime_[0](randomEngine_);
        } else if (Timer_ <= 2400.0f) {
            time_ = spawnTime_[1](randomEngine_);
        } else if (Timer_ <= 3600) {
            time_ = spawnTime_[2](randomEngine_);
        }
    }

    //敵の更新処理
    for (auto& e : e_Manager_->GetEnemies()) {
        e.Update();

        //敵とコア
        if (e.IsCollision(Vector2{ circle_.center.x, circle_.center.y }, circle_.radius)) {
            if (e.GetIsAlive())
                coreHp_--;
            e.SetIsAlive();
            se_playerdamage->Play();
        }

        //敵と弾
        for (auto& b : player_->GetBullet())
            if (e.IsCollision(b.GetPositon(), b.GetRadius().x)) {
                e.SetIsAlive();
                se_enemy->Play();

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

            se_playerstan->Play();
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

            se_playertoutch->Play();
            //めり込みを直す
            player_->SetPosition(player_->GetPositon() + (p_result_[i].penetration * p_result_[i].normal));
            //反射ベクトル更新
            player_->SetReflect(Reflect(player_->GetVelocity(), p_result_[i].normal));

            //反射ベクトルをxだけ反転させるように一時的に宣言
            Vector2 reflect = player_->GetReflect();

            if (!area) { // 左側
                if (player_->GetVelocity().x > 0.0f && player_->GetWallTouch()) {
                    reflect.x = std::abs(reflect.x);
                    reflect.x += 3.0f;
                } else if (player_->GetVelocity().x >= 0.0f) {
                    reflect.x = std::abs(reflect.x);
                } else if (player_->GetVelocity().x <= 0.0f) {
                    reflect.x *= -1.0f;
                }
            }
            if (area) { // 右側
                if (player_->GetVelocity().x < 0.0f && player_->GetWallTouch()) {
                    reflect.x = -std::abs(reflect.x);
                    reflect.x -= 3.0f;
                } else if (player_->GetVelocity().x <= 0.0f) {
                    reflect.x = -std::abs(reflect.x);
                } else if (player_->GetVelocity().x >= 0.0f) {
                    reflect.x *= -1.0f;
                }
            }
            //プレイヤーの速度に掛ける
            player_->SetVelocity(reflect * kCOR);

            player_->SetWallTouch();
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
                        reflect.x += 3.0f;
                    } else if (b.GetVelocity().x >= 0.0f) {
                        reflect.x = std::abs(reflect.x);
                    } else if (b.GetVelocity().x <= 0.0f) {
                        reflect.x *= -1.0f;
                    }
                }
                if (b.GetArea()) { // 右側
                    if (b.GetVelocity().x < 0.0f && b.GetWallTouch()) {
                        reflect.x = -std::abs(reflect.x);
                        reflect.x -= 3.0f;
                    } else if (b.GetVelocity().x <= 0.0f) {
                        reflect.x = -std::abs(reflect.x);
                    } else if (b.GetVelocity().x >= 0.0f) {
                        reflect.x *= -1.0f;
                    }
                }
                //弾の速度に掛ける
                b.SetVelocity(reflect * kCOR);
                b.SetWallTouch();
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