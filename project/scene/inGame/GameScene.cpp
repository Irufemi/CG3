#include "GameScene.h"

#include "../SceneManager.h"
#include "../SceneName.h"
#include "engine/IrufemiEngine.h"
#include "externals/imgui/imgui.h"
#include "engine/Input/InputManager.h"

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
    
    engine_->GetDrawManager()->SetPointLightClass(pointLight_.get());

    spotLight_ = std::make_unique <SpotLightClass>();
    spotLight_->Initialize();

    engine_->GetDrawManager()->SetSpotLightClass(spotLight_.get());

    /// マップチップフィールド
    // マップチップフィールドの生成
    mapChipField_ = std::make_unique<MapChipField>();
    // マップチップフィールドのファイル読み込み
    mapChipField_->LoadMapChipCsv("resources/blocks.csv");

    /// ブロック
    // ブロックの初期化

    /// ブロック
    // ブロックの初期化（Blocksでまとめて管理）
    blocks_ = std::make_unique<Blocks>();
    blocks_->Initialize(camera_.get(), "block.obj");
    GenerateBlocks();
    
    /// 自キャラ
    // 自キャラの生成
    player_ = std::make_shared<Player>();
    // 3Dモデルデータの生成
    modelplayer_ = std::make_unique<ObjClass>();
    modelplayer_->Initialize(camera_.get(),"player.obj");
    // 座標をマップチップ番号で指定
    Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 18);
    // 自キャラの初期化
    player_->Initialize(modelplayer_.get(), camera_.get(), engine->GetInputManager(), playerPosition);
    // マップチップデータのセット
    player_->SetMapChipField(mapChipField_.get());
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


    // 自キャラの更新
    player_->Update();

    // カメラの更新
    if (debugMode) {
        debugCamera_->Update();
        camera_->SetViewMatrix(debugCamera_->GetCamera().GetViewMatrix());
        camera_->SetPerspectiveFovMatrix(debugCamera_->GetCamera().GetPerspectiveFovMatrix());
    } else {
        camera_->Update("Camera");

    }

    //pが押されていたら
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

    player_->Draw();


    // ブロックの描画（Blocksが全インスタンスを描画）
    engine_->ApplyBlocksPSO();
    if (blocks_) { blocks_->Draw(); }

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


void GameScene::GenerateBlocks() {

    // 要素数
    uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
    uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

    // 要素数を変更する
    // 列数を指定(縦方向のブロック数)
    worldtransformBlocks_.resize(numBlockVirtical);
    for (uint32_t i = 0; i < numBlockVirtical; ++i) {
        // 1列の要素数を設定(横方向のブロック数)
        worldtransformBlocks_[i].resize(numBlockHorizontal);
    }

    // ブロックの生成
    // ブロックの生成（Blocks にインスタンスを積む）
    for (uint32_t i = 0; i < numBlockVirtical; ++i) {
        for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
            if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
                Transform* worldTransform = new Transform();
                worldtransformBlocks_[i][j] = worldTransform;
                worldtransformBlocks_[i][j]->translate = mapChipField_->GetMapChipPositionByIndex(j, i);
                // Blocksにもインスタンスとして追加
                if (blocks_) { blocks_->AddInstance(*worldtransformBlocks_[i][j]); }
            }
        }
    }
}