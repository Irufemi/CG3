#pragma once

#include "../source/Texture.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <d3d12.h>

// 前方宣言
class DirectXCommon;


class TextureManager {
private:
    DirectXCommon* dxCommon_ = nullptr;

    std::unordered_map<std::string, std::shared_ptr<Texture>> textures_;

    Microsoft::WRL::ComPtr<ID3D12Resource> whiteTextureResource;
    D3D12_GPU_DESCRIPTOR_HANDLE whiteTextureHandle = { 0 };

public:
    ~TextureManager() {
    }

    void Initialize(DirectXCommon *dxCommon);

    void LoadAllFromFolder(const std::string& folderPath);

    D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(const std::string& name) const;

    std::vector<std::string>GetTextureNames() const;

    void CreateWhiteDummyTexture();

    // ハンドル取得用
    D3D12_GPU_DESCRIPTOR_HANDLE GetWhiteTextureHandle() { return whiteTextureHandle; }

    uint32_t GetSRVIndex()const;
    void AddSRVIndex();

    // テクスチャ名から元サイズを取得（成功時 true）
    bool GetTextureSize(const std::string& name, uint32_t& outWidth, uint32_t& outHeight) const;
};
