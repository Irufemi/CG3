#pragma once

#include "../source/Texture.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <d3d12.h>

// 前方宣言
class DirectXCommon;

class TextureManager {
private:
    DirectXCommon* dxCommon_ = nullptr;

    // ファイルパス(キー) → Texture（共有）
    std::unordered_map<std::string, std::shared_ptr<Texture>> textures_;

    // 白テクスチャ（フォールバック）
    Microsoft::WRL::ComPtr<ID3D12Resource> whiteTextureResource; // 現実装では未使用（必要なら将来使用）
    D3D12_GPU_DESCRIPTOR_HANDLE whiteTextureHandle = { 0 };

public:
    ~TextureManager() = default;

    // まず呼ぶ：DirectXCommon を渡す
    void Initialize(DirectXCommon* dxCommon);

    // フォルダ配下の画像を一括ロード（任意）
    void LoadAllFromFolder(const std::string& folderPath);

    // ファイルパス（または名前）で取得。未ロードならロードしてキャッシュ
    D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(const std::string& name) const;

    // 管理しているキー一覧（デバッグ用）
    std::vector<std::string> GetTextureNames() const;

    // フォールバック白テクスチャを用意（存在しなければ resources/white.png をロード）
    void CreateWhiteDummyTexture();

    // 白テクスチャのSRVハンドル
    D3D12_GPU_DESCRIPTOR_HANDLE GetWhiteTextureHandle() { return whiteTextureHandle; }

    // SRVインデックス（既存API互換：Texture の静的インデックスに委譲）
    uint32_t GetSRVIndex() const;
    void AddSRVIndex();

    // テクスチャ名から元サイズを取得（成功時 true）
    bool GetTextureSize(const std::string& name, uint32_t& outWidth, uint32_t& outHeight) const;
};