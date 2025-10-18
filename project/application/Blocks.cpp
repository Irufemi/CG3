#include "Blocks.h"

#include "source/Texture.h"
#include "function/Function.h"
#include "function/Math.h"
#include "manager/TextureManager.h"
#include "manager/DrawManager.h"
#include "manager/DebugUI.h"
#include "externals/imgui/imgui.h"
#include "engine/directX/DirectXCommon.h"

DebugUI* Blocks::ui_ = nullptr;
TextureManager* Blocks::textureManager_ = nullptr;
DrawManager* Blocks::drawManager_ = nullptr;

void Blocks::Initialize(Camera* camera, const std::string& filename) {
    this->camera_ = camera;

    objModel_ = LoadObjFileM("resources/obj", filename);

    textures_.clear();
    resources_.clear();
    // ここではインスタンスは作らない（AddInstanceで追加）
}

void Blocks::AddInstance(const Transform& t) {
    assert(camera_ && "Blocks::Initialize() が呼ばれていません");

    // 前提: block.obj は単一メッシュ想定。複数メッシュの場合は必要に応じて拡張。
    assert(!objModel_.meshes.empty() && "objModel has no mesh");
    const auto& mesh = objModel_.meshes.front();

    auto res = std::make_unique<D3D12ResourceUtil>();

    // 頂点バッファ
    res->vertexResource_ = res->GetDirectXCommon()->CreateBufferResource(sizeof(VertexData) * mesh.vertices.size());
    res->vertexBufferView_ = D3D12_VERTEX_BUFFER_VIEW{};
    res->vertexBufferView_.BufferLocation = res->vertexResource_->GetGPUVirtualAddress();
    res->vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * mesh.vertices.size());
    res->vertexBufferView_.StrideInBytes = sizeof(VertexData);

    res->vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&res->vertexData_));
    res->vertexDataList_ = mesh.vertices;
    std::memcpy(res->vertexData_, mesh.vertices.data(), sizeof(VertexData) * mesh.vertices.size());

    // マテリアル
    res->materialResource_ = res->GetDirectXCommon()->CreateBufferResource(sizeof(Material));
    res->materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&res->materialData_));
    res->materialData_->color = mesh.material.color;
    res->materialData_->enableLighting = mesh.material.enableLighting;
    res->materialData_->hasTexture = true;
    res->materialData_->lightingMode = 2;
    res->materialData_->uvTransform = mesh.material.uvTransform;
    res->materialData_->shininess = 64.0f;

    // Transform → WVP
    res->transform_ = t;
    res->transformationMatrix_.world = Math::MakeAffineMatrix(res->transform_.scale, res->transform_.rotate, res->transform_.translate);
    res->transformationMatrix_.WVP = Math::Multiply(res->transformationMatrix_.world, Math::Multiply(camera_->GetViewMatrix(), camera_->GetPerspectiveFovMatrix()));
    // 法線変換用：平行移動を除いた World を使う
    Matrix4x4 worldForNormal = res->transformationMatrix_.world;
    worldForNormal.m[3][0] = 0.0f;
    worldForNormal.m[3][1] = 0.0f;
    worldForNormal.m[3][2] = 0.0f;
    worldForNormal.m[3][3] = 1.0f;

    // 逆転置行列を計算
    res->transformationMatrix_.WorldInverseTranspose =
        Math::Transpose(Math::Inverse(worldForNormal));

    res->transformationResource_ = res->GetDirectXCommon()->CreateBufferResource(sizeof(TransformationMatrix));
    res->transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&res->transformationData_));

    // 定数バッファへ全フィールドを書き込む
    *res->transformationData_ = {
        res->transformationMatrix_.WVP,
        res->transformationMatrix_.world,
        res->transformationMatrix_.WorldInverseTranspose
    };

    // テクスチャ
    if (!mesh.material.textureFilePath.empty()) {
        auto tex = std::make_unique<Texture>();
        tex->Initialize(mesh.material.textureFilePath, res->GetDirectXCommon()->GetSrvDescriptorHeap(), res->GetDirectXCommon()->GetCommandList());
        res->textureHandle_ = tex->GetTextureSrvHandleGPU();
        textures_.push_back(std::move(tex)); // 初期化できた時だけ保持
    } else {
        res->materialData_->hasTexture = false;
        // フォールバック（白）
        if (textureManager_) {
            res->textureHandle_ = textureManager_->GetWhiteTextureHandle();
        } else {
            res->textureHandle_ = {}; // nullでも描けるが色計算のみ
        }
    }

    // ライト
    res->directionalLightResource_ = res->GetDirectXCommon()->CreateBufferResource(sizeof(DirectionalLight));
    res->directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&res->directionalLightData_));
    res->directionalLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
    res->directionalLightData_->direction = { 0.0f,-1.0f,0.0f, };
    res->directionalLightData_->intensity = 1.0f;

    res->cameraResource_ = res->GetDirectXCommon()->CreateBufferResource(sizeof(CameraForGPU));
    res->cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&res->cameraData_));
    res->cameraData_->worldPosition = camera_->GetTranslate();

    resources_.push_back(std::move(res));
}

void Blocks::Update(const char* /*objName*/) {
    for (auto& res : resources_) {
        res->UpdateTransform3D(*camera_);
        *res->transformationData_ = { res->transformationMatrix_.WVP, res->transformationMatrix_.world ,res->transformationMatrix_.WorldInverseTranspose };;
        res->materialData_->uvTransform = Math::MakeAffineMatrix(res->uvTransform_.scale, res->uvTransform_.rotate, res->uvTransform_.translate);
        res->directionalLightData_->direction = Math::Normalize(res->directionalLightData_->direction);
    }
}

void Blocks::Draw() {
    for (auto& res : resources_) {
        drawManager_->DrawByVertex(res.get());
    }
}