//各単元で作成される関数のまとめ場所
#include "IrufemiEngine.h"
#include "externals/imgui/imgui.h"

#include "Camera.h"
#include "DebugCamera.h"
#include "DebugUI.h"
#include "DrawManager.h"
#include "TextureManager.h"
#include "Sprite.h"
#include "Triangle.h"
#include "Sphere.h"
#include "Obj.h"
#include "GetBackBufferIndex.h"

/*プロジェクトを作ろう*/

#include <Windows.h>

/*サウンド再生*/

///事前準備

#include <xaudio2.h>

#include "ChunkHeader.h"
#include "RiffHeader.h"
#include "FormatChunk.h"

///Microsoft Media Foundation

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

/*入力デバイス*/

///準備

#define DIRECTINPUT_VERSION 0x0800 //DirectInputのバージョン指定
#include <dinput.h>

/*プロジェクトを作ろう*/

//クライアント領域のサイズ
const int32_t kClientWidth = 1280;
const int32_t kClientHeight = 720;

// タイトル
const std::wstring kTitle = L"CG3_LE2B_11_スエヒロ_コウイチ";

//windowsアプリでのエントリーポint32_tイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    std::unique_ptr<IrufemiEngine> engine = std::make_unique<IrufemiEngine>();
    engine->Initialize(kTitle,kClientWidth, kClientHeight);

    /*三角形を表示しよう*/

    ///ViewportとScissor(シザー)

    //ビューポート
    D3D12_VIEWPORT viewport = D3D12_VIEWPORT{};
    //クライアント領域のサイズと一緒にして画面全体に表示
    viewport.Width = kClientWidth;
    viewport.Height = kClientHeight;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    //シザー矩形
    D3D12_RECT scissorRect = D3D12_RECT{};
    //基本的にビューポートと同じ矩形が構成されるようにする
    scissorRect.left = 0;
    scissorRect.right = kClientWidth;
    scissorRect.top = 0;
    scissorRect.bottom = kClientHeight;

    DebugUI ui{};
    ui.Initialize(engine->GetCommandList(), engine->GetDevice().Get(), engine->GetHwnd(), engine->GetSwapChainDesc(), engine->GetRtvDesc(), engine->GetSrvDescriptorHeap());


    auto drawManager = std::make_unique< DrawManager>();
    drawManager->Initialize(
        engine->GetCommandList(),
        engine->GetCommandQueue(),
        engine->GetSwapChain(),
        engine->GetFence(),
        engine->GetFenceEvent(),
        engine->GetCommandAllocator(),
        engine->GetSrvDescriptorHeap(),
        engine->GetRootSignature(),
        engine->GetGraphicsPipelineState()
    );

    auto textureManager = std::make_unique <TextureManager>();
    textureManager->Initialize(engine->GetDevice().Get(), engine->GetSrvDescriptorHeap(), engine->GetCommandList());
    textureManager->LoadAllFromFolder("resources/");

    auto camera = std::make_unique < Camera>();
    camera->Initialize(kClientWidth, kClientHeight);

    auto debugCamera = std::make_unique < DebugCamera>();
    debugCamera->Initialize(engine->GetInputManager().get());

    Obj obj;
    obj.Initialize(engine->GetDevice().Get(), camera.get(), engine->GetSrvDescriptorHeap(), engine->GetCommandList());

    Sphere sphere;
    sphere.Initialize(engine->GetDevice().Get(), camera.get(), textureManager.get());

    // (3) 初期 BGM を一番最初のカテゴリ／トラックでループ再生
    IXAudio2SourceVoice* bgmVoice = nullptr;
    float bgmVolume = 0.01f;
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
    int selectedCat = 0;   // カテゴリの初期選択インデックス
    int selectedTrack = 0;   // トラックの初期選択インデックス

    //画面の色を設定
    constexpr std::array<float, 4> clearColor = { 0.1f, 0.25f, 0.5f, 1.0f };

    int loadTexture = false;

    int debugMode = false;

    /*ウィンドウを作ろう*/

   ///メインループ

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
            ui.FrameStart();

            /*ウィンドウを作ろう*/

            //ゲームの処理

            /*開発のUIを出そう*/

            ///ImGuiを使う

            //開発用UIの処理。実際に開発用のUIを出す場合はここをゲーム固有の処理に置き換える
            //ImGui::ShowDemoWindow();


#pragma region ゲームの更新部分

            /*テクスチャを切り替えよう*/

            ///SRVを切り替える


            ImGui::Begin("Texture");
            ImGui::SliderInt("allLoadActivate", &loadTexture, 0, 1);
            ImGui::SliderInt("debugMode", &debugMode, 0, 1);
            ImGui::End();
            if (loadTexture) {
                textureManager->LoadAllFromFolder("resources/");
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
            auto cats = engine->GetAudioManager()->GetCategories();
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

            // トラックコンボ
            auto tracks = engine->GetAudioManager()->GetSoundNames(cats[selectedCat]);
            if (ImGui::BeginCombo("BGM Track", tracks[selectedTrack].c_str())) {
                for (int i = 0; i < (int)tracks.size(); ++i) {
                    bool sel = (i == selectedTrack);
                    if (ImGui::Selectable(tracks[i].c_str(), sel)) {
                        selectedTrack = i;
                        if (bgmVoice) {
                            engine->GetAudioManager()->Stop(bgmVoice);
                            bgmVoice = nullptr;
                        }
                        std::string key = cats[selectedCat] + "/" + tracks[i];
                        bgmVoice = engine->GetAudioManager()->Play(
                            engine->GetAudioManager()->GetSoundData(key),
                            true,  // ループ
                            bgmVolume
                        );
                    }
                    if (sel) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            // 音量スライダー
            ImGui::SliderFloat("Volume", &bgmVolume, 0.0f, 1.0f);
            if (bgmVoice) {
                bgmVoice->SetVolume(bgmVolume);
            }

            ImGui::End();
#pragma endregion


            obj.Update();
            sphere.Update();

            /*入力デバイス*/

            ///使い方サンプル

            //数字の0キーが押されていたら
            if (engine->GetInputManager()->isKeyDown(DIK_0)) {
                OutputDebugStringA("hit 0\n"); ///出力ウィンドウに「Hit 0」と表示
            }

#pragma endregion


            ui.QueueDrawCommands();

            /*画面の色を変えよう*/

            ///コマンドを積み込んで確定させる

            //これから書き込むバックバッファのインデックスを取得
            UINT backBufferIndex = GetBackBufferIndex(engine->GetSwapChain());

            /*前後関係を正しくしよう*/

            ///DSVを設定する

            //描画先のRTVとDSVを設定する
            D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = engine->GetDsvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();

            drawManager->PreDraw(
                engine->GetSwapChainResources(backBufferIndex),
                engine->GetRtvHandles(backBufferIndex),
                engine->GetDsvDescriptorHeap(),
                dsvHandle,
                clearColor,
                1.0f,
                0
            );

            //drawManager->DrawObj(viewport, scissorRect, obj);

            drawManager->DrawSphere(viewport, scissorRect, sphere);

            ui.QueuePostDrawCommands();

            drawManager->PostDraw(
                engine->GetSwapChainResources(backBufferIndex),
                engine->GetFenceValue()
            );

        }

    }

    drawManager->Finalize();

    ui.Shutdown();

    /*DirectX12のオブジェクトを解放しよう*/

    ///解放処理

    // GPU待機
    if (engine->GetCommandQueue() && engine->GetFence()) {
        engine->GetCommandQueue()->Signal(engine->GetFence(), ++engine->GetFenceValue());
        if (engine->GetFence()->GetCompletedValue() < engine->GetFenceValue()) {
            engine->GetFence()->SetEventOnCompletion(engine->GetFenceValue(), engine->GetFenceEvent());
            WaitForSingleObject(engine->GetFenceEvent(), INFINITE);
        }
    }

    engine->Finalize();

    return 0;

}