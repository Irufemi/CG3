#include "TextureManager.h"

#include <filesystem>
#include <algorithm>
#include "../engine/directX/DirectXCommon.h"

// Initialize: DirectXCommon を保存し、Texture にも渡す
void TextureManager::Initialize(DirectXCommon* dxCommon) {
    dxCommon_ = dxCommon;
    Texture::SetDirectXCommon(dxCommon_);
}

// 画像拡張子かを簡易判定
static bool IsImageExt(const std::string& extLower) {
    static const char* exts[] = { ".png", ".jpg", ".jpeg", ".bmp", ".tga", ".dds" };
    for (auto* e : exts) {
        if (extLower == e) { return true; }
    }
    return false;
}

// 指定フォルダ配下を走査してロード（キーはフルパス文字列）
void TextureManager::LoadAllFromFolder(const std::string& folderPath) {
    namespace fs = std::filesystem;
    fs::path root(folderPath);
    if (!fs::exists(root)) { return; }

    for (auto& entry : fs::recursive_directory_iterator(root)) {
        if (!entry.is_regular_file()) { continue; }
        auto p = entry.path();
        auto ext = p.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (!IsImageExt(ext)) { continue; }

        const std::string key = p.string();
        // 既にあるならスキップ
        if (textures_.find(key) != textures_.end()) { continue; }

        // 遅延ロードを避けたい場合のみ即ロード
        // ここでは実際にロードしてキャッシュ
        auto tex = std::make_shared<Texture>();
        tex->Initialize(key);
        textures_.emplace(key, std::move(tex));
    }
}

// 取得（未ロードならロードしてキャッシュ）
D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetTextureHandle(const std::string& name) const {
    // 既存キー検索
    auto it = textures_.find(name);
    if (it != textures_.end()) {
        return it->second->GetTextureSrvHandleGPU();
    }

    // constメソッドだがキャッシュを更新したい：mutable 運用にしたくないので
    // const_cast で自分を外して登録（スレッド非安全、単純化のため）
    auto* self = const_cast<TextureManager*>(this);

    auto tex = std::make_shared<Texture>();
    tex->Initialize(name);
    auto handle = tex->GetTextureSrvHandleGPU();
    self->textures_.emplace(name, std::move(tex));
    return handle;
}

std::vector<std::string> TextureManager::GetTextureNames() const {
    std::vector<std::string> keys;
    keys.reserve(textures_.size());
    for (auto& kv : textures_) {
        keys.push_back(kv.first);
    }
    return keys;
}

void TextureManager::CreateWhiteDummyTexture() {
    // 既に作成済みなら何もしない
    if (whiteTextureHandle.ptr != 0) { return; }

    // 最も簡単な実装：既存の白画像ファイルを使う（例: resources/white.png）
    // プロジェクトに用意されていない場合は、作成して追加する実装に差し替えてください。
    const std::string whitePath = "resources/white.png";
    whiteTextureHandle = GetTextureHandle(whitePath);
}

// 既存API互換：Texture 側の静的SRVインデックスに委譲
uint32_t TextureManager::GetSRVIndex() const {
    return Texture::GetStaticSRVIndex();
}
void TextureManager::AddSRVIndex() {
    Texture::AddStaticSRVIndex();
}

bool TextureManager::GetTextureSize(const std::string& name, uint32_t& outWidth, uint32_t& outHeight) const {
    auto it = textures_.find(name);
    if (it == textures_.end()) { return false; }
    outWidth = it->second->GetWidth();
    outHeight = it->second->GetHeight();
    return true;
}