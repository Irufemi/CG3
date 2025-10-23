#include "GameScene.h"

#include "../SceneManager.h"
#include "../SceneName.h"
#include "engine/IrufemiEngine.h"
#include "imgui.h"

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

    isActiveObj_ = false;
    isActiveSprite_ = false;
    isActiveTriangle_ = false;
    isActiveSphere_ = true;
    isActiveStanfordBunny_ = false;
    isActiveUtashTeapot_ = false;
    isActiveMultiMesh_ = false;
    isActiveMultiMaterial_ = false;
    isActiveSuzanne_ = false;
    isActiveFence_ = false;
    isActiveTerrain_ = true;
    isActiveParticle_ = false;

    if (isActiveObj_) {
        obj = std::make_unique <ObjClass>();
        obj->Initialize(camera_.get());
    }
    if (isActiveSprite_) {
        sprite = std::make_unique <Sprite>();
        sprite->Initialize(camera_.get());
    }
    if (isActiveTriangle_) {
        triangle = std::make_unique <TriangleClass>();
        triangle->Initialize(camera_.get());
    }
    if (isActiveSphere_) {
        sphere = std::make_unique <SphereClass>();
        sphere->Initialize(camera_.get());
    }
    if (isActiveStanfordBunny_) {
        stanfordBunny = std::make_unique <ObjClass>();
        stanfordBunny->Initialize(camera_.get(), "bunny.obj");
    }
    if (isActiveUtashTeapot_) {
        utashTeapot = std::make_unique <ObjClass>();
        utashTeapot->Initialize(camera_.get(), "teapot.obj");
    }
    if (isActiveMultiMesh_) {
        multiMesh = std::make_unique <ObjClass>();
        multiMesh->Initialize(camera_.get(), "multiMesh.obj");
    }
    if (isActiveMultiMaterial_) {
        multiMaterial = std::make_unique <ObjClass>();
        multiMaterial->Initialize(camera_.get(), "multiMaterial.obj");
    }
    if (isActiveSuzanne_) {
        suzanne = std::make_unique <ObjClass>();
        suzanne->Initialize(camera_.get(), "suzanne.obj");
    }
    if (isActiveFence_) {
        fence_ = std::make_unique <ObjClass>();
        fence_->Initialize(camera_.get(), "fence.obj");
    }
    if (isActiveTerrain_) {
        terrain_ = std::make_unique <ObjClass>();
        terrain_->Initialize(camera_.get(), "terrain.obj");
    }
    if (isActiveParticle_) {
        particle = std::make_unique <ParticleClass>();
        particle->Initialize(engine_->GetSrvDescriptorHeap(), camera_.get(), engine_->GetTextureManager(), engine_->GetDebugUI(), "resources/circle.png");
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
    // spotLight 
    spotLight_->Debug();

    ImGui::End();

    ImGui::Begin("Activation");
    ImGui::Checkbox("Obj", &isActiveObj_);
    ImGui::Checkbox("Sprite", &isActiveSprite_);
    ImGui::Checkbox("Triangle", &isActiveTriangle_);
    ImGui::Checkbox("Sphere", &isActiveSphere_);
    ImGui::Checkbox("Utash Teapot", &isActiveUtashTeapot_);
    ImGui::Checkbox("Stanford Bunny", &isActiveStanfordBunny_);
    ImGui::Checkbox("MultiMesh", &isActiveMultiMesh_);
    ImGui::Checkbox("MultiMaterial", &isActiveMultiMaterial_);
    ImGui::Checkbox("Suzanne", &isActiveSuzanne_);
    ImGui::Checkbox("Fence", &isActiveFence_);
    ImGui::Checkbox("Terrain", &isActiveTerrain_);
    ImGui::Checkbox("Particle", &isActiveParticle_);
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

    // BGM
    bgm->Update();

    // 3D

    if (isActiveObj_) {
        if (!obj) {
            obj = std::make_unique<ObjClass>();
            obj->Initialize(camera_.get());
        }
        obj->Update("Plane");
    }
    if (isActiveTriangle_) {
        if (!triangle) {
            triangle = std::make_unique<TriangleClass>();
            triangle->Initialize(camera_.get());
        }
        triangle->Update();
    }
    if (isActiveSphere_) {
        if (!sphere) {
            sphere = std::make_unique<SphereClass>();
            sphere->Initialize(camera_.get());
        }
        sphere->Update();
    }
    if (isActiveUtashTeapot_) {
        if (!utashTeapot) {
            utashTeapot = std::make_unique<ObjClass>();
            utashTeapot->Initialize(camera_.get(), "teapot.obj");
        }
        utashTeapot->Update("Utash Teapot");
    }
    if (isActiveStanfordBunny_) {
        if (!stanfordBunny) {
            stanfordBunny = std::make_unique<ObjClass>();
            stanfordBunny->Initialize(camera_.get(), "bunny.obj");
        }
        stanfordBunny->Update("Stanford Bunny");
    }
    if (isActiveMultiMesh_) {
        if (!multiMesh) {
            multiMesh = std::make_unique<ObjClass>();
            multiMesh->Initialize(camera_.get(), "multiMesh.obj");
        }
        multiMesh->Update("MultiMesh");
    }
    if (isActiveMultiMaterial_) {
        if (!multiMaterial) {
            multiMaterial = std::make_unique<ObjClass>();
            multiMaterial->Initialize(camera_.get(), "multiMaterial.obj");
        }
        multiMaterial->Update("MultiMaterial");
    }
    if (isActiveSuzanne_) {
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
    if (isActiveTerrain_) {
        if (!terrain_) {
            terrain_ = std::make_unique<ObjClass>();
            terrain_->Initialize(camera_.get(), "terrain.obj");
        }
        terrain_->Update("Terrain");
    }
    if (isActiveParticle_) {
        if (!particle) {
            particle = std::make_unique <ParticleClass>();
            particle->Initialize(engine_->GetSrvDescriptorHeap(), camera_.get(), engine_->GetTextureManager(), engine_->GetDebugUI());
        }
        particle->Update();
    }

    // 2D

    if (isActiveSprite_) {
        if (!sprite) {
            sprite = std::make_unique<Sprite>();
            sprite->Initialize(camera_.get());
        }
        sprite->Update();
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

    if (isActiveObj_) {
        obj->Draw();
    }
    if (isActiveTriangle_) {
        engine_->GetDrawManager()->DrawByIndex(triangle->GetD3D12Resource());
    }
    if (isActiveSphere_) {
        sphere->Draw();
    }
    if (isActiveUtashTeapot_) {
        utashTeapot->Draw();
    }
    if (isActiveStanfordBunny_) {
        stanfordBunny->Draw();
    }
    if (isActiveMultiMesh_) {
        multiMesh->Draw();
    }
    if (isActiveMultiMaterial_) {
        multiMaterial->Draw();
    }
    if (isActiveSuzanne_) {
        suzanne->Draw();
    }
    if (isActiveFence_) {
        fence_->Draw();
    }
    if (isActiveTerrain_) {
        terrain_->Draw();
    }

    engine_->SetBlend(BlendMode::kBlendModeAdd);
    engine_->SetDepthWrite(PSOManager::DepthWrite::Disable);
    engine_->ApplyParticlePSO();

    if (isActiveParticle_) {
        engine_->GetDrawManager()->DrawParticle(particle.get());
    }


    // 2D

    engine_->SetBlend(BlendMode::kBlendModeNormal);
    engine_->SetDepthWrite(PSOManager::DepthWrite::Enable);
    engine_->ApplySpritePSO();
}