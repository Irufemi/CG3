#pragma once

#include <d3d12.h>
#include <string>
#include <wrl.h>

class Texture {
protected:

    //SRVを生成するDescriptorHeapの場所を決める
    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_{};
    D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_{};
    std::string filePath_;
    ID3D12Resource* textureResource = nullptr;
    ID3D12Resource* intermediateReasource_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;
    ID3D12DescriptorHeap* srvDescriptorHeap_ = nullptr;
    ID3D12GraphicsCommandList* commandList_ = nullptr;
    static uint32_t index_;
public:
    //デストラクタ
    ~Texture() {
        if (textureResource) { textureResource->Release(); textureResource = nullptr; }
        if (intermediateReasource_) { intermediateReasource_->Release(); intermediateReasource_ = nullptr; }
        if (srvDescriptorHeap_) { srvDescriptorHeap_ = nullptr; }
        if (commandList_) { commandList_ = nullptr; }
        if (device_) { device_.Reset();device_ = nullptr; }
    }

    //初期化
    void Initialize(const std::string& filePath, const Microsoft::WRL::ComPtr<ID3D12Device>& device, ID3D12DescriptorHeap* srvDescriptorHeap, ID3D12GraphicsCommandList* commandList);

    //ゲッター

    D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU() { return textureSrvHandleGPU_; }
};

