#include "TextureManager.h"

#include "../function/Function.h"

/*テクスチャを貼ろう*/

#include "../externals/DirectXTex/DirectXTex.h"

/*テクスチャを正しく配置しよう*/

///事前準備

#include "../externals/DirectXTex/d3dx12.h"

#include <filesystem>

uint32_t Texture::index_ = 0;

void TextureManager::Initialize(const Microsoft::WRL::ComPtr<ID3D12Device>& device, ID3D12DescriptorHeap* srvDescriptorHeap, ID3D12GraphicsCommandList* commandList) {
    device_ = device;
    srvDescriptorHeap_ = srvDescriptorHeap;
    commandList_ = commandList;
}

void TextureManager::LoadAllFromFolder(const std::string& folderPath) {

    namespace fs = std::filesystem;

    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            std::string extension = entry.path().extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

            if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".bmp") {
                std::string filename = entry.path().filename().string();
                std::string fullPath = folderPath + "/" + filename;

                std::shared_ptr<Texture> texture = std::make_shared<Texture>();
                texture->Initialize(fullPath, device_.Get(), srvDescriptorHeap_, commandList_);
                textures_[filename] = texture;
            }
        }
    }
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetTextureHandle(const std::string& name) const {
    auto it = textures_.find(name);
    if (it != textures_.end()) {
        return it->second->GetTextureSrvHandleGPU();
    }
    return {};
}

std::vector<std::string> TextureManager::GetTextureNames() const {
    std::vector<std::string> names;
    for (const auto& [name, _] : textures_) {
        names.push_back(name);
    }
    return names;
}
