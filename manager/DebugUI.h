#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>


class DebugUI{

    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;

public: //メンバ関数

    //初期化
    void Initialize(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList, const Microsoft::WRL::ComPtr<ID3D12Device>& device,HWND &hwnd, DXGI_SWAP_CHAIN_DESC1 &swapChainDesc, D3D12_RENDER_TARGET_VIEW_DESC &rtvDesc, ID3D12DescriptorHeap* srvDescriptorHeap);

    //終了処理
    void Shutdown();

    //フレーム開始
    void FrameStart();

    //描画処理に入る前にコマンドを積む
    void QueueDrawCommands();

    //描画処理が終わったタイミングでコマンドを積む
    void QueuePostDrawCommands();

};

