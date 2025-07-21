#include <Windows.h>
#include <cstdint>
#include <algorithm>
#include <memory>
#include <d3d12.h>

#include "IrufemiEngine.h"
#include "externals/imgui/imgui.h"
#include "function/GetBackBufferIndex.h"
#include "IScene.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "SceneName.h"

//クライアント領域のサイズ
const int32_t kClientWidth = 1280;
const int32_t kClientHeight = 720;

// タイトル
const std::wstring kTitle = L"CG3_LE2B_11_スエヒロ_コウイチ";

// シーン名表示配列
static const char* SceneNameStrings[] = { 
    "Title",
    "InGame",
    "End"
};

//windowsアプリでのエントリーポint32_tイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {


    std::unique_ptr<IrufemiEngine> engine = std::make_unique<IrufemiEngine>();
    engine->Initialize(kTitle, kClientWidth, kClientHeight);

    // 1) 初期シーン
    SceneName currentScene = SceneName::inGame;
    std::unique_ptr<IScene> scene = std::make_unique<GameScene>();
    scene->Initialize(engine.get());


    //画面の色を設定
    constexpr std::array<float, 4> clearColor = { 0.1f, 0.25f, 0.5f, 1.0f };

    MSG msg{};
    //ウィンドウの×ボタンが押されるまでループ
    while (msg.message != WM_QUIT) {
        //Windowsにメッセージが来てたら最優先で処理させる
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {

            //入力情報の更新　
            engine->GetInputManager()->Update();

            //
            engine->GetDebugUI()->FrameStart();

            // シーン切り替え用コンボ
            ImGui::Begin("Scene Selector");
            int idx = static_cast<int>(currentScene);
            if (ImGui::Combo("Scene", &idx,
                SceneNameStrings,
                IM_ARRAYSIZE(SceneNameStrings))) {
                SceneName selected = static_cast<SceneName>(idx);
                if (selected != currentScene) {
                    // 3) 切り替え処理
                    currentScene = selected;
                    scene.reset();  // 前シーンの破棄

                    switch (currentScene) {
                    case SceneName::title:
                        scene = std::make_unique<TitleScene>();
                        break;
                    case SceneName::inGame:
                        scene = std::make_unique<GameScene>();
                        break;
                    case SceneName::end:
                        // EndScene を用意していればそこへ
                        break;
                    }
                    scene->Initialize(engine.get());
                }
            }
            ImGui::End();


            // 更新
            scene->Update();

            // 描画処理に入る前にImGui::Renderを積む
            engine->GetDebugUI()->QueueDrawCommands();

            //これから書き込むバックバッファのインデックスを取得
            UINT backBufferIndex = GetBackBufferIndex(engine->GetSwapChain());

            ///DSVを設定する

            //描画先のRTVとDSVを設定する
            D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = engine->GetDsvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();

            engine->GetDrawManager()->PreDraw(
                engine->GetSwapChainResources(backBufferIndex),
                engine->GetRtvHandles(backBufferIndex),
                engine->GetDsvDescriptorHeap(),
                dsvHandle,
                clearColor,
                1.0f,
                0
            );

            // 描画
            scene->Draw();

            // 描画後処理

            engine->GetDebugUI()->QueuePostDrawCommands();

            engine->GetDrawManager()->PostDraw(
                engine->GetSwapChainResources(backBufferIndex),
                engine->GetFenceValue()
            );

        }

    }

    scene.reset();

    return 0;

}