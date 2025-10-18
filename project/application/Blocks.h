#pragma once

#include <d3d12.h>
#include <string>
#include "../camera/Camera.h"
#include "../source/D3D12ResourceUtil.h"
#include "math/ObjModel.h"
#include <wrl.h>
#include <cstdint>
#include <memory>
#include <vector>
#include <cassert>

// 前方宣言
class TextureManager;
class DrawManager;
class DebugUI;

class Blocks {
    ObjModel objModel_;
    std::vector<std::unique_ptr<Texture>> textures_;                 // メッシュが持つテクスチャ（必要に応じて）
    std::vector<std::unique_ptr<D3D12ResourceUtil>> resources_;      // 各ブロック用の描画リソース（1個=1描画分）

#pragma region 外部参照
    Camera* camera_ = nullptr;
    static DebugUI* ui_;
    static TextureManager* textureManager_;
    static DrawManager* drawManager_;
#pragma endregion

public:
    ~Blocks() = default;

    // OBJの読み込みと使用カメラの設定（インスタンスはここでは作らない）
    void Initialize(Camera* camera, const std::string& filename = "block.obj");

    // ブロックを1つ追加（Transformを指定）
    void AddInstance(const Transform& t);

    // 一括更新・描画
    void Update(const char* objName = " ");
    void Draw();

    // 便利
    size_t Count() const { return resources_.size(); }
    void Clear() { resources_.clear(); textures_.clear(); }

    // 位置/回転/スケール（必要なら）
    const Vector3& GetPosition(uint32_t index = 0) const { return resources_[index]->transform_.translate; }
    void SetPosition(const Vector3& position, uint32_t index = 0) { resources_[index]->transform_.translate = position; }

    const Vector3& GetRotate(uint32_t index = 0) const { return resources_[index]->transform_.rotate; }
    void SetRotate(const Vector3& rotate, uint32_t index = 0) { for (auto& res : resources_) { res->transform_.rotate = rotate; } }

    const Vector3& GetScale(uint32_t index = 0) const { return resources_[index]->transform_.scale; }
    void SetScale(const Vector3& scale) { for (auto& res : resources_) { res->transform_.scale = scale; } }

    const Transform& GetTransform(uint32_t index = 0) const { return resources_[index]->transform_; }
    void SetTransform(Transform transform) { for (auto& res : resources_) { res->transform_ = transform; } }

    const TransformationMatrix& GetTransformationMatrix(uint32_t index = 0) const { return resources_[index]->transformationMatrix_; }
    void SetTransformationMatrix(TransformationMatrix mtx, uint32_t index = 0) { resources_[index]->transformationMatrix_ = mtx; }

    static void SetTextureManager(TextureManager* texM) { textureManager_ = texM; }
    static void SetDrawManager(DrawManager* drawM) { drawManager_ = drawM; }
    static void SetDebugUI(DebugUI* ui) { ui_ = ui; }
};