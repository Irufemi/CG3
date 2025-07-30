#include "GameScene.h"

#include "../engine/IrufemiEngine.h"

#include <algorithm>


// 初期化
void GameScene::Initialize(IrufemiEngine* engine) {

    // 参照したものをコピー
    // エンジン
    this->engine_ = engine;

    camera = std::make_unique <Camera>();
    camera->Initialize(engine->GetClientWidth(), engine->GetClientHeight());

    debugCamera = std::make_unique <DebugCamera>();
    debugCamera->Initialize(engine->GetInputManager());
    debugMode = false;

    isActiveObj = false;
    isActiveSprite = false;
    isActiveSphere = true;
    isActiveStanfordBunny = false;
    isActiveUtashTeapot = false;
    isActiveMultiMesh = false;
    isActiveMultiMaterial = false;
    isActiveSuzanne = false;


    if (isActiveObj) {
        obj = std::make_unique <Obj>();
        obj->Initialize(engine_->GetDevice(), camera.get(), engine_->GetSrvDescriptorHeap(), engine->GetCommandList(), engine_->GetDebugUI(), engine_->GetTextureManager());
    }
    if (isActiveSprite) {
        sprite = std::make_unique <Sprite>();
        sprite->Initialize(engine_->GetDevice(), camera.get(), engine_->GetTextureManager(), engine_->GetDebugUI());
    }
    if (isActiveSphere) {
        sphere = std::make_unique <Sphere>();
        sphere->Initialize(engine_->GetDevice(), camera.get(), engine_->GetTextureManager(), engine_->GetDebugUI());
    }
    if (isActiveStanfordBunny) {
        stanfordBunny = std::make_unique <Obj>();
        stanfordBunny->Initialize(engine_->GetDevice(), camera.get(), engine_->GetSrvDescriptorHeap(), engine->GetCommandList(), engine_->GetDebugUI(), engine_->GetTextureManager(), "bunny.obj");
    }
    if (isActiveUtashTeapot) {
        utashTeapot = std::make_unique <Obj>();
        utashTeapot->Initialize(engine_->GetDevice(), camera.get(), engine_->GetSrvDescriptorHeap(), engine->GetCommandList(), engine_->GetDebugUI(), engine_->GetTextureManager(), "teapot.obj");
    }
    if (isActiveMultiMesh) {
        multiMesh = std::make_unique <Obj>();
        multiMesh->Initialize(engine_->GetDevice(), camera.get(), engine_->GetSrvDescriptorHeap(), engine->GetCommandList(), engine_->GetDebugUI(), engine_->GetTextureManager(), "multiMesh.obj");
    }
    if (isActiveMultiMaterial) {
        multiMaterial = std::make_unique <Obj>();
        multiMaterial->Initialize(engine_->GetDevice(), camera.get(), engine_->GetSrvDescriptorHeap(), engine->GetCommandList(), engine_->GetDebugUI(), engine_->GetTextureManager(), "multiMaterial.obj");
    }
    if (isActiveSuzanne) {
        suzanne = std::make_unique <Obj>();
        suzanne->Initialize(engine_->GetDevice(), camera.get(), engine_->GetSrvDescriptorHeap(), engine->GetCommandList(), engine_->GetDebugUI(), engine_->GetTextureManager(), "suzanne.obj");
    }

    bgm = std::make_unique<Bgm>();
    bgm->Initialize(engine_->GetAudioManager());
    bgm->PlayFirstTrack();
}

// 更新
void GameScene::Update() {

    ImGui::Begin("Activation");
    ImGui::Checkbox("Obj", &isActiveObj);
    ImGui::Checkbox("Sprite", &isActiveSprite);
    ImGui::Checkbox("Sphere", &isActiveSphere);
    ImGui::Checkbox("Utash Teapot", &isActiveUtashTeapot);
    ImGui::Checkbox("Stanford Bunny", &isActiveStanfordBunny);
    ImGui::Checkbox("MultiMesh", &isActiveMultiMesh);
    ImGui::Checkbox("MultiMaterial", &isActiveMultiMaterial);
    ImGui::Checkbox("Suzanne", &isActiveSuzanne);
    ImGui::End();

    ImGui::Begin("Texture");
    if (ImGui::Button("allLoadActivate")) {
        engine_->GetTextureManager()->LoadAllFromFolder("resources/");

    }
    ImGui::Checkbox("debugMode", &debugMode);
    ImGui::End();

    if (debugMode) {
        debugCamera->Update();
        camera->SetViewMatrix(debugCamera->GetCamera().GetViewMatrix());
        camera->SetPerspectiveFovMatrix(debugCamera->GetCamera().GetPerspectiveFovMatrix());
    } else {
        camera->Update("Camera");

    }

    // BGM
    bgm->Update();

    // 3D

    if (isActiveObj) {
        if (!obj) {
            obj = std::make_unique<Obj>();
            obj->Initialize(engine_->GetDevice(), camera.get(), engine_->GetSrvDescriptorHeap(), engine_->GetCommandList(), engine_->GetDebugUI(), engine_->GetTextureManager());
        }
        obj->Update("Plane");
    }
    if (isActiveSphere) {
        if (!sphere) {
            sphere = std::make_unique<Sphere>();
            sphere->Initialize(engine_->GetDevice(), camera.get(), engine_->GetTextureManager(), engine_->GetDebugUI());
        }
        sphere->Update();
    }
    if (isActiveUtashTeapot) {
        if (!utashTeapot) {
            utashTeapot = std::make_unique<Obj>();
            utashTeapot->Initialize(engine_->GetDevice(), camera.get(), engine_->GetSrvDescriptorHeap(), engine_->GetCommandList(), engine_->GetDebugUI(), engine_->GetTextureManager(), "teapot.obj");
        }
        utashTeapot->Update("Utash Teapot");
    }
    if (isActiveStanfordBunny) {
        if (!stanfordBunny) {
            stanfordBunny = std::make_unique<Obj>();
            stanfordBunny->Initialize(engine_->GetDevice(), camera.get(), engine_->GetSrvDescriptorHeap(), engine_->GetCommandList(), engine_->GetDebugUI(), engine_->GetTextureManager(), "bunny.obj");
        }
        stanfordBunny->Update("Stanford Bunny");
    }
    if (isActiveMultiMesh) {
        if (!multiMesh) {
            multiMesh = std::make_unique<Obj>();
            multiMesh->Initialize(engine_->GetDevice(), camera.get(), engine_->GetSrvDescriptorHeap(), engine_->GetCommandList(), engine_->GetDebugUI(), engine_->GetTextureManager(), "multiMesh.obj");
        }
        multiMesh->Update("MultiMesh");
    }
    if (isActiveMultiMaterial) {
        if (!multiMaterial) {
            multiMaterial = std::make_unique<Obj>();
            multiMaterial->Initialize(engine_->GetDevice(), camera.get(), engine_->GetSrvDescriptorHeap(), engine_->GetCommandList(), engine_->GetDebugUI(), engine_->GetTextureManager(), "multiMaterial.obj");
        }
        multiMaterial->Update("MultiMaterial");
    }
    if (isActiveSuzanne) {
        if (!suzanne) {
            suzanne = std::make_unique<Obj>();
            suzanne->Initialize(engine_->GetDevice(), camera.get(), engine_->GetSrvDescriptorHeap(), engine_->GetCommandList(), engine_->GetDebugUI(), engine_->GetTextureManager(), "suzanne.obj");
        }
        suzanne->Update("Suzanne");
    }

    // 2D

    if (isActiveSprite) {
        if (!sprite) {
            sprite = std::make_unique<Sprite>();
            sprite->Initialize(engine_->GetDevice(), camera.get(), engine_->GetTextureManager(), engine_->GetDebugUI());
        }
        sprite->Update();
    }

    /*入力デバイス*/

    ///使い方サンプル

    //数字の0キーが押されていたら
    if (engine_->GetInputManager()->IsKeyDown('0')) {
        OutputDebugStringA("hit 0\n"); ///出力ウィンドウに「Hit 0」と表示
    }

}

// 描画
void GameScene::Draw() {

    // 3D

    if (isActiveObj) {
        obj->Draw(engine_->GetDrawManager(), engine_->GetViewport(), engine_->GetScissorRect());
    }
    if (isActiveSphere) {
        engine_->GetDrawManager()->DrawSphere(engine_->GetViewport(), engine_->GetScissorRect(), sphere.get());
    }
    if (isActiveUtashTeapot) {
        utashTeapot->Draw(engine_->GetDrawManager(), engine_->GetViewport(), engine_->GetScissorRect());
    }
    if (isActiveStanfordBunny) {
        stanfordBunny->Draw(engine_->GetDrawManager(), engine_->GetViewport(), engine_->GetScissorRect());
    }
    if (isActiveMultiMesh) {
        multiMesh->Draw(engine_->GetDrawManager(), engine_->GetViewport(), engine_->GetScissorRect());
    }
    if (isActiveMultiMaterial) {
        multiMaterial->Draw(engine_->GetDrawManager(), engine_->GetViewport(), engine_->GetScissorRect());
    }
    if (isActiveSuzanne) {
        suzanne->Draw(engine_->GetDrawManager(), engine_->GetViewport(), engine_->GetScissorRect());
    }

    // 2D

    if (isActiveSprite) {
        engine_->GetDrawManager()->DrawSprite(engine_->GetViewport(), engine_->GetScissorRect(), sprite.get());
    }
}
