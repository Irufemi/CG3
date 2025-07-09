#pragma once

#include <d3d12.h>
#include <vector>
#include <cstdint>
#include "manager/TextureManager.h"
#include "math/VertexData.h"
#include "math/Material.h"
#include "math/Matrix4x4.h"
#include "math/TransformationMatrix.h"
#include "math/DirectionalLight.h"
#include "math/Transform.h"
#include "Camera.h"
#include <wrl.h>

class Sprite {
protected:

    Transform transform_ = {
        {1.0f,1.0f,1.0f},   //scale
        {0.0f,0.0f,0.0f},   //rotate
        {0.0f,0.0f,0.0f}    //translate
    };

    // 頂点データ
    std::vector<VertexData> vertexDataList_{};
    //頂点バッファ
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

    D3D12_INDEX_BUFFER_VIEW indexBufferView_;
    //頂点リソース
    VertexData* vertexData_ = nullptr;

    uint32_t* indexData_ = nullptr;
    //マテリアルデータ
    Material* materialData_ = nullptr;

    Transform uvTransform_{
        {1.0f,1.0f,1.0f},
        {0.0f,0.0f,0.0f},
        {0.0f,0.0f,0.0f}
    };

    Matrix4x4 uvTransformMatrix_;

    // ワールド行列
    Matrix4x4 worldMatrix_{};

    Matrix4x4 wvpMatrix_{};

    TransformationMatrix* wvpData_ = nullptr;

    DirectionalLight* directionalLightData_ = nullptr;

    ID3D12Resource* vertexResource_ = nullptr;

    ID3D12Resource* indexResource_ = nullptr;
    // 行列用定数バッファ
    ID3D12Resource* wvpResource_ = nullptr;
    // 色用定数バッファ
    ID3D12Resource* materialResource_ = nullptr;
    //平行光源用バッファ
    ID3D12Resource* directionalLightResource_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;

    Camera* camera_ = nullptr;

    TextureManager* textureManager_ = nullptr;

    bool isRotateY_ = true;

    D3D12_GPU_DESCRIPTOR_HANDLE textureHandle_ = {};

    int selectedTextureIndex_ = 0;


public: //メンバ関数
    //デストラクタ
    ~Sprite() {
        if (vertexResource_) { vertexResource_->Release(); vertexResource_ = nullptr; }
        if (indexResource_) { indexResource_->Release(); indexResource_ = nullptr; }
        if (wvpResource_) { wvpResource_->Release(); wvpResource_ = nullptr; }
        if (materialResource_) { materialResource_->Release(); materialResource_ = nullptr; }
        if (directionalLightResource_) { directionalLightResource_->Release(); directionalLightResource_ = nullptr; }
        if (device_) { device_.Reset(); }
    }

    //初期化
    void Initialize(const Microsoft::WRL::ComPtr<ID3D12Device>& device, Camera* camera, TextureManager* textureManager, const std::string& textureName = "uvChecker.png");
    //更新
    void Update();

    D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() { return vertexBufferView_; }
    D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() { return indexBufferView_; }
    ID3D12Resource* GetIndexResource() { return indexResource_; }
    ID3D12Resource* GetWvpResource() { return wvpResource_; }
    ID3D12Resource* GetMaterialResource() { return materialResource_; }
    ID3D12Resource* GetDirectionalLightResource() { return directionalLightResource_; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle() { return textureHandle_; }

};

