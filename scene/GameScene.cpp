#include "GameScene.h"

#include "../engine/IrufemiEngine.h"

#include <algorithm>



// 初期化
void GameScene::Initialize(IrufemiEngine * engine) {

    // 参照したものをコピー
    // エンジン
    this->engine_ = engine;

    camera = std::make_unique <Camera>();
    camera->Initialize(engine->GetClientWidth(), engine->GetClientHeight());

    debugCamera = std::make_unique <DebugCamera>();
    debugCamera->Initialize(engine->GetInputManager());
    debugMode = false;

    triangle = std::make_unique <Triangle>();
    triangle->Initialize(engine->GetDevice(), camera.get(), engine->GetTextureManager());
    isActiveTriangle = false;

    sprite = std::make_unique <Sprite>();
    sprite->Initialize(engine->GetDevice(), camera.get(), engine->GetTextureManager());
    isActiveSprite = false;

    sphere = std::make_unique <Sphere>();
    sphere->Initialize(engine->GetDevice(), camera.get(), engine->GetTextureManager());
    isActiveSphere = true;

    obj = std::make_unique <Obj>();
    obj->Initialize(engine->GetDevice(), camera.get(), engine->GetSrvDescriptorHeap(), engine->GetCommandList(), "fence.obj");
    isActiveObj = false;

    // (3) 初期 BGM を一番最初のカテゴリ／トラックでループ再生
    {
        auto categories = engine->GetAudioManager()->GetCategories();           // 登録済みカテゴリ一覧取得 :contentReference[oaicite:1]{index=1}
        if (!categories.empty()) {
            auto names = engine->GetAudioManager()->GetSoundNames(categories[0]); // カテゴリ[0] のトラック一覧取得 :contentReference[oaicite:2]{index=2}
            if (!names.empty()) {
                std::string key = categories[0] + "/" + names[0];
                bgmVoice = engine->GetAudioManager()->Play(
                    engine->GetAudioManager()->GetSoundData(key),  // Sound オブジェクト取得 :contentReference[oaicite:3]{index=3}
                    true,    // loop = true → 無限ループ再生
                    bgmVolume
                );
            }
        }
    }
}

// 更新
void GameScene::Update() {

    ImGui::Begin("Activation");
    ImGui::Checkbox("Triangle", &isActiveTriangle);
    ImGui::Checkbox("Sprite", &isActiveSprite);
    ImGui::Checkbox("Sphere", &isActiveSphere);
    ImGui::Checkbox("Obj", &isActiveObj);
    ImGui::End();

    ImGui::Begin("Texture");
    ImGui::SliderInt("allLoadActivate", &loadTexture, 0, 1);
    ImGui::Checkbox("debugMode", &debugMode);
    ImGui::End();


    if (loadTexture) {
        engine_->GetTextureManager()->LoadAllFromFolder("resources/");
        loadTexture = 0;
    }
    if (debugMode) {
        debugCamera->Update();
        camera->SetViewMatrix(debugCamera->GetCamera().GetViewMatrix());
        camera->SetPerspectiveFovMatrix(debugCamera->GetCamera().GetPerspectiveFovMatrix());
    } else {
        camera->Update("Camera");

    }

#pragma region BGM
    ImGui::Begin("Audio Settings");

    // カテゴリコンボ
    auto cats = engine_->GetAudioManager()->GetCategories();
    if (!cats.empty()) {
        // selectedCat を範囲内にクランプ
        selectedCat = std::clamp(selectedCat, 0, (int)cats.size() - 1);
        if (ImGui::BeginCombo("Category", cats[selectedCat].c_str())) {
            for (int i = 0; i < (int)cats.size(); ++i) {
                bool sel = (i == selectedCat);
                if (ImGui::Selectable(cats[i].c_str(), sel)) {
                    selectedCat = i;
                    selectedTrack = 0;
                }
                if (sel) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }


    // トラックコンボ
    if (!cats.empty()) {
        auto tracks = engine_->GetAudioManager()->GetSoundNames(cats[selectedCat]);
        if (!tracks.empty()) {
            selectedTrack = std::clamp(selectedTrack, 0, (int)tracks.size() - 1);
            if (ImGui::BeginCombo("BGM Track", tracks[selectedTrack].c_str())) {
                for (int i = 0; i < (int)tracks.size(); ++i) {
                    bool sel = (i == selectedTrack);
                    if (ImGui::Selectable(tracks[i].c_str(), sel)) {
                        selectedTrack = i;
                        if (bgmVoice) {
                            engine_->GetAudioManager()->Stop(bgmVoice);
                        }
                        std::string key = cats[selectedCat] + "/" + tracks[i];
                        bgmVoice = engine_->GetAudioManager()->Play(
                            engine_->GetAudioManager()->GetSoundData(key),
                            true,  // ループ
                            bgmVolume
                        );
                    }
                    if (sel) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }
    }

    // 音量スライダー
    ImGui::SliderFloat("Volume", &bgmVolume, 0.0f, 1.0f);
    if (bgmVoice) {
        bgmVoice->SetVolume(bgmVolume);
    }

    ImGui::End();
#pragma endregion

    // 3D

    if (isActiveTriangle) {
        triangle->Update();
    }
    if (isActiveSphere) {
        sphere->Update();
    }
    if (isActiveObj) {
        obj->Update();
    }

    // 2D

    if (isActiveSprite) {
        sprite->Update();
    }

    /*入力デバイス*/

    ///使い方サンプル

    //数字の0キーが押されていたら
    if (engine_->GetInputManager()->isKeyDown(DIK_0)) {
        OutputDebugStringA("hit 0\n"); ///出力ウィンドウに「Hit 0」と表示
    }

}

// 更新
void GameScene::Draw() {

    // 3D

    if (isActiveTriangle) {
        engine_->GetDrawManager()->DrawByIndex(engine_->GetViewport(), engine_->GetScissorRect(), triangle->GetD3D12Resource());
    }
    if (isActiveSphere) {
        engine_->GetDrawManager()->DrawSphere(engine_->GetViewport(), engine_->GetScissorRect(), sphere.get());
    }
    if (isActiveObj) {
        engine_->GetDrawManager()->DrawObj(engine_->GetViewport(), engine_->GetScissorRect(), obj.get());
    }

    // 2D

    if (isActiveSprite) {
        engine_->GetDrawManager()->DrawSprite(engine_->GetViewport(), engine_->GetScissorRect(), sprite.get());
    }
}