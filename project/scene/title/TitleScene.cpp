#include "TitleScene.h"

#include "../SceneManager.h"
#include "../SceneName.h"
#include "engine/IrufemiEngine.h"
#include <imgui.h>

// 初期化
void TitleScene::Initialize(IrufemiEngine* engine) {
    (void)engine;

    engine_ = engine;

    camera_ = std::make_unique <Camera>();
    camera_->Initialize(engine_->GetClientWidth(), engine_->GetClientHeight());
    camera_->SetTranslate(Vector3{ 0.0f,0.0f,-10.0f });

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

    bgm_ = std::make_unique<Bgm>();
    bgm_->Initialize("resources/bgm/BGM_Title.mp3");
    bgm_->PlayFixed();

    se_select = std::make_unique<Se>();
    se_select->Initialize("resources/se/SE_Select.mp3");
    se_select->SetVolume(0.01f);

    text_dan_1 = std::make_unique<ObjClass>();
    text_dan_1 ->Initialize(camera_.get(), "text_dan_01.obj");
    text_dan_2 = std::make_unique<ObjClass>();
    text_dan_2->Initialize(camera_.get(), "text_dan_02.obj");
    text_ri = std::make_unique<ObjClass>();
    text_ri->Initialize(camera_.get(), "text_ri.obj");
    text_sa = std::make_unique<ObjClass>();
    text_sa->Initialize(camera_.get(), "text_sa.obj");
    text_i = std::make_unique<ObjClass>();
    text_i->Initialize(camera_.get(), "text_i.obj");
    text_ku = std::make_unique<ObjClass>();
    text_ku->Initialize(camera_.get(), "text_ku.obj");
    text_ru = std::make_unique<ObjClass>();
    text_ru->Initialize(camera_.get(), "text_ru.obj");
}

// 更新
void TitleScene::Update() {

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

    text_dan_1->Update("text_dan_1");
    text_dan_2->Update("text_dan_2");
    text_ri->Update("text_ri");
    text_sa->Update("text_sa");
    text_i->Update("text_i");
    text_ku->Update("text_ku");
    text_ru->Update("text_ru");

    //エンターキーが押されていたら
    if (engine_->GetInputManager()->IsKeyPressed(VK_SPACE)) {
        se_select->Play();


        //if (g_SceneManager) {
        //    g_SceneManager->Request(SceneName::inGame);
        //}
    }

}

// 更新
void TitleScene::Draw() {

    // 3D

    engine_->SetBlend(BlendMode::kBlendModeNormal);
    engine_->SetDepthWrite(PSOManager::DepthWrite::Enable);
    engine_->ApplyPSO();

    text_dan_1->Draw();
    text_dan_2->Draw();
    text_ri->Draw();
    text_sa->Draw();
    text_i->Draw();
    text_ku->Draw();
    text_ru->Draw();

}