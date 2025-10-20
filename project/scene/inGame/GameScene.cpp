#include "GameScene.h"

#include "../SceneManager.h"
#include "../SceneName.h"
#include "engine/IrufemiEngine.h"
#include "externals/imgui/imgui.h"

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

    isActiveObj = false;
    isActiveSprite = false;
    isActiveTriangle = false;
    isActiveSphere = false;
    isActiveStanfordBunny = false;
    isActiveUtashTeapot = false;
    isActiveMultiMesh = false;
    isActiveMultiMaterial = false;
    isActiveSuzanne = false;
    isActiveFence_ = false;
    isActiveParticle = true;

    if (isActiveObj) {
        obj = std::make_unique <ObjClass>();
        obj->Initialize(camera_.get());
    }
    if (isActiveSprite) {
        sprite = std::make_unique <Sprite>();
        sprite->Initialize(camera_.get());
    }
    if (isActiveTriangle) {
        triangle = std::make_unique <TriangleClass>();
        triangle->Initialize(camera_.get());
    }
    if (isActiveSphere) {
        sphere = std::make_unique <SphereClass>();
        sphere->Initialize(camera_.get());
    }
    if (isActiveStanfordBunny) {
        stanfordBunny = std::make_unique <ObjClass>();
        stanfordBunny->Initialize(camera_.get(), "bunny.obj");
    }
    if (isActiveUtashTeapot) {
        utashTeapot = std::make_unique <ObjClass>();
        utashTeapot->Initialize(camera_.get(), "teapot.obj");
    }
    if (isActiveMultiMesh) {
        multiMesh = std::make_unique <ObjClass>();
        multiMesh->Initialize(camera_.get(), "multiMesh.obj");
    }
    if (isActiveMultiMaterial) {
        multiMaterial = std::make_unique <ObjClass>();
        multiMaterial->Initialize(camera_.get(), "multiMaterial.obj");
    }
    if (isActiveSuzanne) {
        suzanne = std::make_unique <ObjClass>();
        suzanne->Initialize(camera_.get(), "suzanne.obj");
    }
    if (isActiveFence_) {
        fence_ = std::make_unique <ObjClass>();
        fence_->Initialize(camera_.get(), "fence.obj");
    }
    if (isActiveParticle) {
        particle = std::make_unique <ParticleClass>();
        particle->Initialize(engine_->GetSrvDescriptorHeap(), camera_.get(), engine_->GetTextureManager(), engine_->GetDebugUI(), "circle.png");
    }

    bgm = std::make_unique<Bgm>();
    bgm->Initialize(engine_->GetAudioManager());
    bgm->PlayFirstTrack();
}

// 更新
void GameScene::Update() {

#if defined(_DEBUG) || defined(DEVELOPMENT)
    
    ImGui::Begin("GameScene");
    // pointLight 
    pointLight_->Debug();

    ImGui::End();

    ImGui::Begin("Activation");
    ImGui::Checkbox("Obj", &isActiveObj);
    ImGui::Checkbox("Sprite", &isActiveSprite);
    ImGui::Checkbox("Triangle", &isActiveTriangle);
    ImGui::Checkbox("Sphere", &isActiveSphere);
    ImGui::Checkbox("Utash Teapot", &isActiveUtashTeapot);
    ImGui::Checkbox("Stanford Bunny", &isActiveStanfordBunny);
    ImGui::Checkbox("MultiMesh", &isActiveMultiMesh);
    ImGui::Checkbox("MultiMaterial", &isActiveMultiMaterial);
    ImGui::Checkbox("Suzanne", &isActiveSuzanne);
    ImGui::Checkbox("Fence", &isActiveFence_);
    ImGui::Checkbox("Particle", &isActiveParticle);
    ImGui::End();

    ImGui::Begin("Texture");
    if (ImGui::Button("allLoadActivate")) {
        engine_->GetTextureManager()->LoadAllFromFolder("resources/");
    }
    ImGui::Checkbox("debugMode", &debugMode);
    ImGui::End();

#endif // _DEBUG

    if (debugMode) {
        debugCamera_->Update();
        camera_->SetViewMatrix(debugCamera_->GetCamera().GetViewMatrix());
        camera_->SetPerspectiveFovMatrix(debugCamera_->GetCamera().GetPerspectiveFovMatrix());
    } else {
        camera_->Update("Camera");

    }

    // BGM
    bgm->Update();

    // 3D

    if (isActiveObj) {
        if (!obj) {
            obj = std::make_unique<ObjClass>();
            obj->Initialize(camera_.get());
        }
        obj->Update("Plane");
    }
    if (isActiveTriangle) {
        if (!triangle) {
            triangle = std::make_unique<TriangleClass>();
            triangle->Initialize(camera_.get());
        }
        triangle->Update();
    }
    if (isActiveSphere) {
        if (!sphere) {
            sphere = std::make_unique<SphereClass>();
            sphere->Initialize(camera_.get());
        }
        sphere->Update();
    }
    if (isActiveUtashTeapot) {
        if (!utashTeapot) {
            utashTeapot = std::make_unique<ObjClass>();
            utashTeapot->Initialize(camera_.get(), "teapot.obj");
        }
        utashTeapot->Update("Utash Teapot");
    }
    if (isActiveStanfordBunny) {
        if (!stanfordBunny) {
            stanfordBunny = std::make_unique<ObjClass>();
            stanfordBunny->Initialize(camera_.get(), "bunny.obj");
        }
        stanfordBunny->Update("Stanford Bunny");
    }
    if (isActiveMultiMesh) {
        if (!multiMesh) {
            multiMesh = std::make_unique<ObjClass>();
            multiMesh->Initialize(camera_.get(),  "multiMesh.obj");
        }
        multiMesh->Update("MultiMesh");
    }
    if (isActiveMultiMaterial) {
        if (!multiMaterial) {
            multiMaterial = std::make_unique<ObjClass>();
            multiMaterial->Initialize(camera_.get(), "multiMaterial.obj");
        }
        multiMaterial->Update("MultiMaterial");
    }
    if (isActiveSuzanne) {
        if (!suzanne) {
            suzanne = std::make_unique<ObjClass>();
            suzanne->Initialize(camera_.get(), "suzanne.obj");
        }
        suzanne->Update("Suzanne");
    }
    if (isActiveFence_) {
        if (!fence_) {
            fence_ = std::make_unique<ObjClass>();
            fence_->Initialize(camera_.get(), "fence.obj");
        }
        fence_->Update("Fence");
    }
    if (isActiveParticle) {
        if (!particle) {
            particle = std::make_unique <ParticleClass>();
            particle->Initialize(engine_->GetSrvDescriptorHeap(), camera_.get(), engine_->GetTextureManager(), engine_->GetDebugUI());
        }
        particle->Update();
    }

    // 2D

    if (isActiveSprite) {
        if (!sprite) {
            sprite = std::make_unique<Sprite>();
            sprite->Initialize(camera_.get());
        }
        sprite->Update();
    }



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

    if (isActiveObj) {
        obj->Draw();
    }
    if (isActiveTriangle) {
        engine_->GetDrawManager()->DrawByIndex(triangle->GetD3D12Resource());
    }
    if (isActiveSphere) {
        sphere->Draw();
    }
    if (isActiveUtashTeapot) {
        utashTeapot->Draw();
    }
    if (isActiveStanfordBunny) {
        stanfordBunny->Draw();
    }
    if (isActiveMultiMesh) {
        multiMesh->Draw();
    }
    if (isActiveMultiMaterial) {
        multiMaterial->Draw();
    }
    if (isActiveSuzanne) {
        suzanne->Draw();
    }
    if (isActiveFence_) {
        fence_->Draw();
    }

    engine_->SetBlend(BlendMode::kBlendModeAdd);
    engine_->SetDepthWrite(PSOManager::DepthWrite::Disable);
    engine_->ApplyParticlePSO();

    if (isActiveParticle) {
        engine_->GetDrawManager()->DrawParticle(particle.get());
    }

    // 2D

    engine_->SetBlend(BlendMode::kBlendModeNormal);
    engine_->SetDepthWrite(PSOManager::DepthWrite::Enable);
    engine_->ApplySpritePSO();

    if (isActiveSprite) {
        sprite->Draw();
    }
}
