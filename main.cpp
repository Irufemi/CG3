//各単元で作成される関数のまとめ場所

#include "Function.h"
#include "Camera.h"
#include "DebugUI.h"
#include "DebugCamera.h"
#include "Material.h"
#include "DirectionalLight.h"
#include "Triangle.h"
#include "Texture.h"
#include "Sprite.h"
#include "Sphere.h"
#include "DrawManager.h"
#include "TextureManager.h"
#include "AudioManager.h"
#include "InputManager.h"
#include "DrawImGui.h"
#include "GetBackBufferIndex.h"
#include "Obj.h"
#include "Log.h"



/*プロジェクトを作ろう*/

#include <Windows.h>


/*ウィンドウを作ろう*/

//int32_tを使うためにinclude
#include <cstdint>

/*ログを出そう*/

#include <string>

#include <format>

//ファイルやディレクトリに関する操作を行うライブラリ
#include <filesystem>

//ファイルに書いたり読んだりするライブラリ
#include <fstream>

//時間を扱うライブラリ
#include <chrono>

/*DirectX12を初期化しよう*/

#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

/*CrashHandler*/

//Debug用のあれやこれやを使えるようにする
#include <DbgHelp.h>
#pragma comment(lib,"Dbghelp.lib")

/*DirectX12のオブジェクトを解放しよう*/

#include <dxgidebug.h>
#pragma comment(lib,"dxguid.lib")

/*三角形を表示しよう*/

#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")
#include "Vector4.h"

#include <algorithm> //std::fill,copy

/*三角形を動かそう*/

#include "Matrix4x4.h"
#include "Transform.h"
#include "Math.h"

/*テクスチャを貼ろう*/

#include "VertexData.h"

/*ComPtr*/

///ComPtr(コムポインタ)

#include <wrl.h>

#include "D3DResourceLeakChecker.h"

/*サウンド再生*/

///事前準備

#include <xaudio2.h>
#pragma comment (lib,"xaudio2.lib")

#include "ChunkHeader.h"
#include "RiffHeader.h"
#include "FormatChunk.h"

///Microsoft Media Foundation

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

/*入力デバイス*/

///準備

#define DIRECTINPUT_VERSION 0x0800 //DirectInputのバージョン指定
#include <dinput.h>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

/*プロジェクトを作ろう*/


//windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    D3DResourceLeakChecker leakCheck;

    /*テクスチャを貼ろう*/

    ///COMの初期化

    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    assert(SUCCEEDED(hr));

    // AudioManagerの生成・Media Foundationの初期化
    auto audioManager = std::make_unique<AudioManager>();
    audioManager->StartUp();

    /*CrashHandler*/
    SetUnhandledExceptionFilter(ExportDump);

    // ログを出せるようにする
    std::unique_ptr<Log> log = std::make_unique<Log>();
    log->Initialize();


    /*ウィンドウを作ろう*/

    ///ウィンドウクラスを登録する

    WNDCLASS wc{};
    //ウィンドウプロシージャ
    wc.lpfnWndProc = WindowProc;
    //ウィンドウクラス名(なんでもいい)
    wc.lpszClassName = L"CG2WindowClass";
    //インスタンスハンドル
    wc.hInstance = GetModuleHandle(nullptr);
    //カーソル
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    //ウィンドウクラスを登録する
    RegisterClass(&wc);

    ///ウィンドウサイズを決める

    //クライアント領域のサイズ
    const int32_t kClientWidth = 1280;
    const int32_t kClientHeight = 720;

    //ウィンドウサイズを表す構造体にクライアント領域を入れる
    RECT wrc = { 0,0,kClientWidth ,kClientHeight };

    //クライアント領域をもとに実際のサイズにwrcを変更してもらう
    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

    ///ウィンドウを生成して表示

    //ウィンドウの生成
    HWND hwnd = CreateWindow(
        wc.lpszClassName,		//利用するクラス名
        L"CG3",			        //タイトルバーの文字(何でも良い)
        WS_OVERLAPPEDWINDOW,	//よく見るウィンドウスタイル
        CW_USEDEFAULT,			//表示X座標(windowsに任せる)
        CW_USEDEFAULT,			//表示Y座標(windowsに任せる)
        wrc.right - wrc.left,	//ウィンドウ横幅
        wrc.bottom - wrc.top,	//ウィンドウ縦幅
        nullptr,				//親ウィンドウハンドル
        nullptr,				//メニューハンドル
        wc.hInstance,			//インスタンスハンドル
        nullptr					//オプション
    );

    /*エラー放置ダメ、絶対*/

    ///DebugLayer(デバッグレイヤー)

#ifdef _DEBUG
    ID3D12Debug1* debugController = nullptr;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
        //デバッグレイヤーを有効化する
        debugController->EnableDebugLayer();
        //さらにGPU側でもチェックを行うようにする
        debugController->SetEnableGPUBasedValidation(TRUE);
    }
#endif

    /*ウィンドウを作ろう*/

    ///ウィンドウを生成して表示

    //ウィンドウを表示する
    ShowWindow(hwnd, SW_SHOW);

    /*DirectX12を初期化しよう*/

    ///DXGIFactoryの生成

    //DXGIファクトリーの生成
    Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
    //HRESULTはWndows系のエラーコードであり、
    //関数が成功したかどうかをSUCCEEDEDマクロで判定できる
    hr = CreateDXGIFactory(IID_PPV_ARGS(dxgiFactory.GetAddressOf()));
    //初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、どうにもできない場合が多いのでassertにしておく
    assert(SUCCEEDED(hr));

    ///使用するアダプタ(GPU)を決定する

    //使用するアダプタ用の変数。最初にnullptrを入れておく
    Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter = nullptr;
    //良い順にアダプタを頼む
    for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(useAdapter.GetAddressOf())) != DXGI_ERROR_NOT_FOUND; i++) {
        //アダプター情報を取得する
        DXGI_ADAPTER_DESC3 adapterDesc{};
        hr = useAdapter->GetDesc3(&adapterDesc);
        assert(SUCCEEDED(hr)); //取得できないのは一大事
        //ソフトウェアアダプタでなければ採用!
        if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
            //採用したアダプタの情報をログに出力。wstringの方なので注意
            OutPutLog(log->GetLogStream(), ConvertString(std::format(L"use Adapter:{}\n", adapterDesc.Description)));
            break;
        }
        useAdapter = nullptr; //ソフトウェアアダプタの場合は見なかったことにする
    }
    //適切なアダプタが見つからなかったので起動できない
    assert(useAdapter != nullptr);

    ///D3D12Deviceの生成

    Microsoft::WRL::ComPtr<ID3D12Device> device = nullptr;
    //機能レベルとログ出力用の文字列
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
    };
    const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
    //高い順に生成できるか試してく
    for (size_t i = 0; i < _countof(featureLevels); ++i) {
        //採用したアダプターでデバイスを生成
        hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(device.GetAddressOf()));
        //指定した機能レベルでデバイスが生成できたかを確認
        if (SUCCEEDED(hr)) {
            //生成できたのでログ出力を行ってループを抜ける
            OutPutLog(log->GetLogStream(), std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
            break;
        }
    }
    //デバイス生成がうまくいかなかったので起動できない
    assert(device != nullptr);
    OutPutLog(log->GetLogStream(), "Complete create D3D12Device!!!\n"); //初期化完了のログを出す

    InputManager* inputManager = new InputManager;
    inputManager->Initialize(wc, hwnd);

    // 生成が完了したのでuseAdapterを解放
    if (useAdapter) { useAdapter.Reset(); }

    // AudioManagerの初期化
    audioManager->Initialize();

    // "resources"フォルダから音声ファイルをすべてロード
    audioManager->LoadAllSoundsFromFolder("resources/");

    /*エラー放置ダメ、絶対*/

    ///エラー・警告、即ち停止

#ifdef _DEBUG
    ID3D12InfoQueue* infoQueue = nullptr;
    if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
        //ヤバイエラー時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        //エラー時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        //警告時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

        ///エラーと警告の抑制(Windowsの不具合によるエラー表記などは解消不能。その場合に停止させないよう設定を行う)

        //抑制するメッセージのID
        D3D12_MESSAGE_ID denyIds[] = {
            //Windows11でのDXGIデバッグプレイヤーとDX12デバッグプレイヤーの相互作用バグによるエラーメッセージ
            //https://stackoverflow.com/questions/69805245/directXx-12-application-is-crashing-in-windows-11
            D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
        };
        //抑制するレベル
        D3D12_MESSAGE_SEVERITY severties[] = { D3D12_MESSAGE_SEVERITY_INFO };
        D3D12_INFO_QUEUE_FILTER filter{};
        filter.DenyList.NumIDs = _countof(denyIds);
        filter.DenyList.pIDList = denyIds;
        filter.DenyList.NumSeverities = _countof(severties);
        filter.DenyList.pSeverityList = severties;
        //指定したメッセージの表示を抑制する
        infoQueue->PushStorageFilter(&filter);

        ///エラー・警告、即ち停止

        //解散
        infoQueue->Release();
    }
#endif

    /*画面の色を変えよう*/

    ///CommandQueueを生成する

    //コマンドキューを生成する
    ID3D12CommandQueue* commandQueue = nullptr;
    D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
    hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
    //コマンドキューの生成がうまくいかないので起動できない
    assert(SUCCEEDED(hr));

    ///CommandListを生成する

    //コマンドアロケータを生成する
    ID3D12CommandAllocator* commandAllocator = nullptr;
    hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
    //コマンドアロケータの生成がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));

    //コマンドリストを生成する
    ID3D12GraphicsCommandList* commandList = nullptr;
    hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList));
    //コマンドリストの生成がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));

    ///SwapChainを生成する

    //スワップチェーンを生成する
    IDXGISwapChain4* swapChain = nullptr;
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
    swapChainDesc.Width = kClientWidth; //画面の幅。ウィンドウのクライアント領域を同じものにしておく
    swapChainDesc.Height = kClientHeight; //画面の高さ。ウィンドウのクライアント領域を同じものにしておく
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //色の形式
    swapChainDesc.SampleDesc.Count = 1; //マルチサンプルしない
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //描画のターゲットとして利用する
    swapChainDesc.BufferCount = 2; //ダブルバッファ
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; //モニタにうつしたら、中身を破棄
    //コマンドキュー、ウィンドウハンドル、設定を渡して生成する
    hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue, hwnd, &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&swapChain));
    assert(SUCCEEDED(hr));

    // 作成したのでdxgiFactoryを解放
    if (dxgiFactory) { dxgiFactory.Reset(); }


    /*テクスチャを切り替えよう*/

    //DescriptorSize

    const uint32_t descriptorSizeSRV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    const uint32_t descriptorSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    const uint32_t descriptorSizeDSV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

    /*開発用のUIを出そう*/

    //作成関数を使う

    //RTV用のヒープでディスクリプタの数は2。RTVはShader内で触るものではないので、ShaderVisibleはfalse
    ID3D12DescriptorHeap* rtvDescriptorHeap = CreateDescriptorHeap(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

    //SRV用のヒープでディスクリプタの数は128。SSRVはShader内で触るものなので、ShaderVisibleはtrue
    ID3D12DescriptorHeap* srvDescriptorHeap = CreateDescriptorHeap(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

    /*画面の色を変えよう*/

    ///SwapChainからResourceを引っ張ってくる

    //SwapChainからResourceを引っ張ってくる
    ID3D12Resource* swapChainResources[2] = { nullptr };
    hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
    //うまく取得できなければ起動できない
    assert(SUCCEEDED(hr));
    hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
    assert(SUCCEEDED(hr));

    ///RTVを作る

    //RTVの設定
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; //出力結果をSRGBに変換して書き込む
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; //2Dテクスチャとして書き込む
    //ディスクリプタの先頭を取得する
    D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    //RTVを2つ作るのでディスクリプタを2つ用意
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
    //まず1つ目を作る。1つ目は最初のところに作る。作る場所をこちらで指定してあげる必要がある
    rtvHandles[0] = rtvStartHandle;
    device->CreateRenderTargetView(swapChainResources[0], &rtvDesc, rtvHandles[0]);
    //2つ目のディスクリプタハンドルを得る(自力で)
    rtvHandles[1].ptr = rtvHandles[0].ptr + device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    //2つ目を作る
    device->CreateRenderTargetView(swapChainResources[1], &rtvDesc, rtvHandles[1]);

    /*前後関係を正しくしよう*/

    //DepthtencilTexturreをウィンドウのサイズで作成
    ID3D12Resource* depthStencilResource = CreateDepthStencilTextureResource(device.Get(), kClientWidth, kClientHeight);

    ///DepthStencilView(DSV)

    //DSV用のヒープでディスクリプタの数は1。DSVはShader内で触るものではないので、ShaderVisibleはfalse
    ID3D12DescriptorHeap* dsvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

    //DSVの設定
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //Format。基本的にはResourceに合わせる
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; //2dTexture
    //DSVHeapの先頭にDSVをつける
    device->CreateDepthStencilView(depthStencilResource, &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

    /*完璧な画面クリアを目指して*/

    ///FenceとEventを生成する

    //初期値0でFenceを作る
    ID3D12Fence* fence = nullptr;
    uint64_t fenceValue = 0;
    hr = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    assert(SUCCEEDED(hr));

    //FenceのSignalを待つためのイベントを作成する
    HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    assert(fenceEvent != nullptr);

    /*三角形を表示しよう*/

    ///DXCの初期化

    //dxcCompilerを初期化
    Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils = nullptr;
    Microsoft::WRL::ComPtr <IDxcCompiler3> dxcCompiler = nullptr;
    hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(dxcUtils.GetAddressOf()));
    assert(SUCCEEDED(hr));
    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(dxcCompiler.GetAddressOf()));
    assert(SUCCEEDED(hr));

    //現時点でincludeはしないが、includeに対応するための設定を行っておく
    Microsoft::WRL::ComPtr <IDxcIncludeHandler> includeHandler = nullptr;
    hr = dxcUtils->CreateDefaultIncludeHandler(includeHandler.GetAddressOf());
    assert(SUCCEEDED(hr));

    ///RootSignatureを生成する

    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
    descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    /*テクスチャを貼ろう*/

    ///RootSignatureを書き換える

    ///DescriptorRange

    D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
    descriptorRange[0].BaseShaderRegister = 0; //0からは始まる
    descriptorRange[0].NumDescriptors = 1; //数は1つ
    descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; //SRVを使う
    descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; //offsetを自動計算

    ///DescriptorTable

    D3D12_ROOT_PARAMETER rootParameters[4] = {};
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //CBVを使う
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
    rootParameters[0].Descriptor.ShaderRegister = 0; //レジスタ番号0を使う
    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //CBVを使う
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; //VertexShaderで使う
    rootParameters[1].Descriptor.ShaderRegister = 0; //レジスタ番号0を使う
    rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //descriptorTableを使う
    rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
    rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange; //Tableの中身の配列を指定
    rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); //Tableで利用する数

    /*LambertianReflectance*/

    ///平行光源をShaderで使う

    rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //CBVを使う
    rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
    rootParameters[3].Descriptor.ShaderRegister = 1; //レジスタ番号1を使う


    /*テクスチャを貼ろう*/

    ///Samplerの設定

    D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
    staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; //バイリニアフィルタ
    staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; //0~1の範囲外をリピート
    staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; //比較しない
    staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; //ありったけのMIpmapを使う
    staticSamplers[0].ShaderRegister = 0; //レジスタ番号0を使う
    staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
    descriptionRootSignature.pStaticSamplers = staticSamplers;
    descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

    /*三角形の色を変えよう*/

    ///RootParameter

    descriptionRootSignature.pParameters = rootParameters; //ルートパラメータ配列へのポインタ
    descriptionRootSignature.NumParameters = _countof(rootParameters); //配列の長さ

    ///register(解説)

    //rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //b0のbと一致する
    //rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    //rootParameters[0].Descriptor.ShaderRegister = 0; //b0のbと一致する。もしb11と紐づけたいなら11となる

    /*三角形を表示しよう*/

    ///RootSignatureを生成する

    //シリアライズしてバイナリにする
    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob>  errorBlob = nullptr;
    hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, signatureBlob.GetAddressOf(), errorBlob.GetAddressOf());
    if (FAILED(hr)) {
        OutPutLog(log->GetLogStream(), reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
        assert(false);
    }
    //バイナリを元に生成
    ID3D12RootSignature* rootSignature = nullptr;
    hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
    assert(SUCCEEDED(hr));

    // 生成が完了したのでsignatureBlob、errorBlobを解放
    if (signatureBlob) { signatureBlob.Reset(); }
    if (errorBlob) { errorBlob.Reset(); }

    /*テクスチャを貼ろう*/

    ///InputLayoutの拡張

    D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
    inputElementDescs[0].SemanticName = "POSITION";
    inputElementDescs[0].SemanticIndex = 0;
    inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    inputElementDescs[1].SemanticName = "TEXCOORD";
    inputElementDescs[1].SemanticIndex = 0;
    inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    /*LambertianReflectance*/

    ///法線の定義を追加する

    inputElementDescs[2].SemanticName = "NORMAL";
    inputElementDescs[2].SemanticIndex = 0;
    inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    /*三角形を表示しよう*/

    ///InputLayoutの設定を行う

    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
    inputLayoutDesc.pInputElementDescs = inputElementDescs;
    inputLayoutDesc.NumElements = _countof(inputElementDescs);

    ///BlendStateの設定を行う

    //BlendStateの設定
    D3D12_BLEND_DESC blendDesc{};
    //すべての色要素を書き込む
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    ///RasterizerStateの設定を行う

    //RasterizerStateの設定
    D3D12_RASTERIZER_DESC rasterizerDesc{};
    //裏面(時計回り)を表示しない
    rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
    //三角形の中を塗りつぶす
    rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

    ///ShaderをCompileする

    //Shaderをコンパイルする
    Microsoft::WRL::ComPtr <IDxcBlob> vertexShaderBlob = CompileShader(L"Object3D.VS.hlsl", L"vs_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get(), log->GetLogStream());
    assert(vertexShaderBlob != nullptr);

    Microsoft::WRL::ComPtr <IDxcBlob> pixelShaderBlob = CompileShader(L"Object3D.PS.hlsl", L"ps_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get(), log->GetLogStream());
    assert(pixelShaderBlob != nullptr);

    // コンパイルが完了したのでdxcUtils、dxcCompiler、includeHandlerを解放
    if (dxcUtils) { dxcUtils.Reset(); }
    if (dxcCompiler) { dxcCompiler.Reset(); }
    if (includeHandler) { includeHandler.Reset(); }

    /*前後関係を正しくしよう*/

    ///DepthStencilStateの設定を行う

    //DepthStencilStateの設定
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
    //Depthの機能を有効化する
    depthStencilDesc.DepthEnable = true;
    //書き込みします
    depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    //比較関数はLessEqual。つまり、近ければ描画される
    depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

    /*三角形を表示しよう*/

    ///PSOを生成する

    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
    graphicsPipelineStateDesc.pRootSignature = rootSignature; // RootSignature
    graphicsPipelineStateDesc.InputLayout = inputLayoutDesc; // InputLayout
    graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),vertexShaderBlob->GetBufferSize() }; // VertexShader
    graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),pixelShaderBlob->GetBufferSize() }; // PixelShader

    // 生成が完了したのでvertexShaderBlobを解放
    if (vertexShaderBlob) { vertexShaderBlob.Reset(); }
    if (pixelShaderBlob) { pixelShaderBlob.Reset(); }

    /*前後関係を正しくしよう*/

    ///DepthStencilStateの設定を行う

    //DepthStencilの設定
    graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
    graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    /*三角形を表示しよう*/

    ///PSOを生成する

    graphicsPipelineStateDesc.BlendState = blendDesc; // BlendState
    graphicsPipelineStateDesc.RasterizerState = rasterizerDesc; // RasterizerState
    //書き込むRTVの情報
    graphicsPipelineStateDesc.NumRenderTargets = 1;
    graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    //利用するトロポジ(形状)のタイプ。三角形
    graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    //どのように画面に色を打ち込むかの設定(気にしなくて良い)
    graphicsPipelineStateDesc.SampleDesc.Count = 1;
    graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
    //実際に生成
    ID3D12PipelineState* graphicsPipelineState = nullptr;
    hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
    assert(SUCCEEDED(hr));

    //頂点リソース用のヒープを生成
    D3D12_HEAP_PROPERTIES uploadHeapProperties{};
    uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; //UploadHeapを使う
    //頂点リソースの設定
    D3D12_RESOURCE_DESC vertexResourceDesc{};
    //バッファリソース、テクスチャの場合はまた別の設定をする
    vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    //vertexResourceDesc.Width = sizeof(Vector4) * 3; //リソースのサイズ。今回はVector4を3頂点分

    /*テクスチャを貼ろう*/

    ///頂点データの拡張
    vertexResourceDesc.Width = sizeof(VertexData) * 3; //リソースのサイズ。今回はVertexDataを3頂点分

    /*三角形を表示しよう*/

    ///PSOを生成する

    //バッファの場合はこれらは1にする決まり
    vertexResourceDesc.Height = 1;
    vertexResourceDesc.DepthOrArraySize = 1;
    vertexResourceDesc.MipLevels = 1;
    vertexResourceDesc.SampleDesc.Count = 1;
    //バッファの場合はこれにする決まり
    vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    //実際に頂点リソースを作る
    Microsoft::WRL::ComPtr<ID3D12Resource> dummyVertexResource = nullptr;
    hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(dummyVertexResource.GetAddressOf()));
    assert(SUCCEEDED(hr));

    // 頂点リソースを作ったので解放
    if (dummyVertexResource) { dummyVertexResource.Reset(); }

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
    ui.Initialize(commandList, device.Get(), hwnd, swapChainDesc, rtvDesc, srvDescriptorHeap);


    auto* drawManager = new DrawManager;
    drawManager->Initialize(
        commandList,
        commandQueue,
        swapChain,
        fence,
        fenceEvent,
        commandAllocator,
        srvDescriptorHeap,
        rootSignature,
        graphicsPipelineState
    );

    auto* textureManager = new TextureManager;
    textureManager->Initialize(device.Get(), srvDescriptorHeap, commandList);
    textureManager->LoadAllFromFolder("resources/");

    auto* camera = new Camera;
    camera->Initialize(kClientWidth, kClientHeight);

    auto* debugCamera = new DebugCamera;
    debugCamera->Initialize(inputManager);

    Obj obj;
    obj.Initialize(device, camera, srvDescriptorHeap, commandList);

    Sphere sphere;
    sphere.Initialize(device, camera, textureManager);

    // (3) 初期 BGM を一番最初のカテゴリ／トラックでループ再生
    IXAudio2SourceVoice* bgmVoice = nullptr;
    float bgmVolume = 0.01f;
    {
        auto categories = audioManager->GetCategories();           // 登録済みカテゴリ一覧取得 :contentReference[oaicite:1]{index=1}
        if (!categories.empty()) {
            auto names = audioManager->GetSoundNames(categories[0]); // カテゴリ[0] のトラック一覧取得 :contentReference[oaicite:2]{index=2}
            if (!names.empty()) {
                std::string key = categories[0] + "/" + names[0];
                bgmVoice = audioManager->Play(
                    audioManager->GetSoundData(key),  // Sound オブジェクト取得 :contentReference[oaicite:3]{index=3}
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
            inputManager->Update();

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
            auto cats = audioManager->GetCategories();
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
            auto tracks = audioManager->GetSoundNames(cats[selectedCat]);
            if (ImGui::BeginCombo("BGM Track", tracks[selectedTrack].c_str())) {
                for (int i = 0; i < (int)tracks.size(); ++i) {
                    bool sel = (i == selectedTrack);
                    if (ImGui::Selectable(tracks[i].c_str(), sel)) {
                        selectedTrack = i;
                        if (bgmVoice) {
                            audioManager->Stop(bgmVoice);
                            bgmVoice = nullptr;
                        }
                        std::string key = cats[selectedCat] + "/" + tracks[i];
                        bgmVoice = audioManager->Play(
                            audioManager->GetSoundData(key),
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
            if (inputManager->isKeyDown(DIK_0)) {
                OutputDebugStringA("hit 0\n"); ///出力ウィンドウに「Hit 0」と表示
            }

#pragma endregion


            ui.QueueDrawCommands();

            /*画面の色を変えよう*/

            ///コマンドを積み込んで確定させる

            //これから書き込むバックバッファのインデックスを取得
            UINT backBufferIndex = GetBackBufferIndex(swapChain);

            /*前後関係を正しくしよう*/

            ///DSVを設定する

            //描画先のRTVとDSVを設定する
            D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

            drawManager->PreDraw(
                swapChainResources[backBufferIndex],
                rtvHandles[backBufferIndex],
                dsvDescriptorHeap,
                dsvHandle,
                clearColor,
                1.0f,
                0
            );

            //drawManager->DrawObj(viewport, scissorRect, obj);

            drawManager->DrawSphere(viewport, scissorRect, sphere);

            ui.QueuePostDrawCommands();

            drawManager->PostDraw(
                swapChainResources[backBufferIndex],
                fenceValue
            );

        }

    }

    drawManager->Finalize();

    ui.Shutdown();

    /*DirectX12のオブジェクトを解放しよう*/

    ///解放処理

    // GPU待機
    if (commandQueue && fence) {
        commandQueue->Signal(fence, ++fenceValue);
        if (fence->GetCompletedValue() < fenceValue) {
            fence->SetEventOnCompletion(fenceValue, fenceEvent);
            WaitForSingleObject(fenceEvent, INFINITE);
        }
    }

    delete camera;
    delete debugCamera;
    delete drawManager;
    delete textureManager;
    delete inputManager;



    // OSハンドル
    CloseHandle(fenceEvent);

    // リソース（VBやPSOなど）
    if (graphicsPipelineState) { graphicsPipelineState->Release(); graphicsPipelineState = nullptr; }
    if (rootSignature) { rootSignature->Release(); rootSignature = nullptr; }

    // テクスチャ・バッファ系（あれば）
    if (depthStencilResource) { depthStencilResource->Release(); depthStencilResource = nullptr; }

    // ディスクリプタヒープ
    if (rtvDescriptorHeap) { rtvDescriptorHeap->Release(); rtvDescriptorHeap = nullptr; }
    if (srvDescriptorHeap) { srvDescriptorHeap->Release(); srvDescriptorHeap = nullptr; }
    if (dsvDescriptorHeap) { dsvDescriptorHeap->Release(); dsvDescriptorHeap = nullptr; }

    // スワップチェーンのバッファ
    if (swapChainResources[0]) { swapChainResources[0]->Release(); swapChainResources[0] = nullptr; }
    if (swapChainResources[1]) { swapChainResources[1]->Release(); swapChainResources[1] = nullptr; }

    // コマンド系
    if (commandList) { commandList->Release(); commandList = nullptr; }
    if (commandAllocator) { commandAllocator->Release(); commandAllocator = nullptr; }
    if (commandQueue) { commandQueue->Release(); commandQueue = nullptr; }

    // フェンス
    if (fence) { fence->Release(); fence = nullptr; }

    // スワップチェーン
    if (swapChain) { swapChain->Release(); swapChain = nullptr; }

    // Device（一番最後に解放）
    if (device) { device.Reset(); }

    

    // Debug (一番最後の最後)
#ifdef _DEBUG
    if (debugController) { debugController->Release(); debugController = nullptr; }
#endif

    // ウィンドウ
    CloseWindow(hwnd);

    // COMアンイニシャライズ
    CoUninitialize();

    return 0;

}