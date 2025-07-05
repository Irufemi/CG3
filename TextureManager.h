#pragma once

#include "Texture.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <d3d12.h>
#include <wrl.h>


class TextureManager {
private:
    Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;
    ID3D12DescriptorHeap* srvDescriptorHeap_ = nullptr;
    ID3D12GraphicsCommandList* commandList_ = nullptr;

    std::unordered_map<std::string, std::shared_ptr<Texture>> textures_;


public:
    ~TextureManager() {
        textures_.clear();
        if (device_) { device_.Reset(); device_ = nullptr; }
        if (srvDescriptorHeap_) { srvDescriptorHeap_ = nullptr; }
        if (commandList_) { commandList_ = nullptr; }
    }

    void Initialize(const Microsoft::WRL::ComPtr<ID3D12Device>& device, ID3D12DescriptorHeap* srvDescriptorHeap,
        ID3D12GraphicsCommandList* commandList);
        
    void LoadAllFromFolder(const std::string& folderPath);

    D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(const std::string& name) const;

    std::vector<std::string>GetTextureNames() const;
};
