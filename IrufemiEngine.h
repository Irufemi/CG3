#pragma once

#include "D3DResourceLeakChecker.h"
#include "manager/AudioManager.h"
#include "manager/InputManager.h"
#include <memory>
#include "Log.h"
#include <Windows.h>
#include <d3d12.h>
#include <dxcapi.h>
#include <wrl.h>
#include <dxgi1_6.h>

// 前方宣言

class IrufemiEngine {
public: // メンバ列挙型

    enum class BlendMode {
        // !< ブレンドなし
        kBlendModeNone,
        // !< 通常のブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
        kBlendModeNormal,
        // !< 加算。 Src * SrcA + Dest * 1
        kBlendModeAdd,
        // !< 減算。 Dest * 1 - Src * SrcA
        kBlendModeSubtract,
        // !< 乗算。 Src * 0 + Dest * Src
        kBlendModeMultiply,
        // !< スクリーン。 Src * (1 - Dest) + dest * 1
        kBlendModeScreen,
        // 利用してはいけない
        kCountOfBlendMode,
    };

private: // メンバ変数

    // --- Debug & Logging ---

    // リソース解放リークチェック
    D3DResourceLeakChecker leakCheck_;

    ID3D12Debug1* debugController_ = nullptr;

    // ログ
    std::unique_ptr<Log> log_ = nullptr;

    // --- Window ---

    HWND hwnd_;

    // --- Manager ---

    // AudioManager
    std::unique_ptr< AudioManager>audioManager_ = nullptr;

    // InputManager
    std::unique_ptr <InputManager> inputManager_ = nullptr;

    // --- D3D Device & Core ---

    Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;

    ID3D12CommandQueue* commandQueue_ = nullptr;

    ID3D12CommandAllocator* commandAllocator_ = nullptr;

    ID3D12GraphicsCommandList* commandList_ = nullptr;

    // --- SwapChain & Render Targets ---

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};

    IDXGISwapChain4* swapChain = nullptr;

    ID3D12Resource* swapChainResources[2] = { nullptr };

    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];

    // --- Descriptor Heaps ---

    ID3D12DescriptorHeap* rtvDescriptorHeap = nullptr;

    ID3D12DescriptorHeap* srvDescriptorHeap = nullptr;

    ID3D12DescriptorHeap* dsvDescriptorHeap = nullptr;

    // --- Depth & Pipeline State ---

    ID3D12Resource* depthStencilResource = nullptr;

    ID3D12RootSignature* rootSignature = nullptr;

    ID3D12PipelineState* graphicsPipelineState = nullptr;

    // --- Synchronization --

    ID3D12Fence* fence = nullptr;

    uint64_t fenceValue = 0;

    HANDLE fenceEvent = nullptr;

public: // メンバ関数
    /// <summary>
    ///  初期化
    /// </summary>
    void Initialize(const std::wstring& title, const int32_t& clientWidth = 1280, const int32_t& clientHeight = 720);

    /// <summary>
    /// 解放
    /// </summary>
    void Finalize();

public: // ゲッター

    ID3D12GraphicsCommandList* GetCommandList() { return this->commandList_; }
    Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() { return this->device_.Get(); }
    HWND& GetHwnd() { return this->hwnd_; }
    DXGI_SWAP_CHAIN_DESC1& GetSwapChainDesc() { return this->swapChainDesc_; }
    D3D12_RENDER_TARGET_VIEW_DESC& GetRtvDesc() { return this->rtvDesc; }
    ID3D12DescriptorHeap* GetSrvDescriptorHeap() { return this->srvDescriptorHeap; }
    ID3D12CommandQueue* GetCommandQueue() { return this->commandQueue_; }
    IDXGISwapChain4* GetSwapChain() { return this->swapChain; }
    ID3D12Fence* GetFence() { return this->fence; }
    HANDLE& GetFenceEvent() { return this->fenceEvent; }
    ID3D12CommandAllocator* GetCommandAllocator() { return this->commandAllocator_; }
    ID3D12RootSignature* GetRootSignature() { return this->rootSignature; }
    ID3D12PipelineState* GetGraphicsPipelineState() { return this->graphicsPipelineState; }
    ID3D12DescriptorHeap* GetDsvDescriptorHeap() { return this->dsvDescriptorHeap; }
    ID3D12Resource* GetSwapChainResources(UINT index) { return this->swapChainResources[index]; }
    D3D12_CPU_DESCRIPTOR_HANDLE& GetRtvHandles(UINT index) { return this->rtvHandles[index]; }
    uint64_t& GetFenceValue() { return this->fenceValue; }
    std::unique_ptr<AudioManager>& GetAudioManager() { return this->audioManager_; }
    std::unique_ptr<InputManager>& GetInputManager() { return this->inputManager_; }

public: // セッター
    void AddFenceValue(uint32_t index) { this->fenceValue += index; }
};

