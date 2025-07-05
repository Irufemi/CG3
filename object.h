#pragma once

#include <vector>
#include <d3d12.h>
#include <cstdint>

#include "Matrix4x4.h"
#include "Transform.h"
#include "VertexData.h"
#include "Material.h"
#include "TransformationMatrix.h"
#include "DirectionalLight.h"
#include "TextureManager.h"

#include "Function.h"

class Resource {
protected: //メンバ変数

#pragma region Vertex

    // 頂点データリスト
    std::vector<VertexData> vertexDataList_{};

    //頂点データ
    VertexData* vertexData_ = nullptr;

#pragma endregion

#pragma region Index

    //頂点インデックスリスト
    std::vector<uint32_t> indexDataList_{};

    //頂点インデックス
    uint32_t* indexData_ = nullptr;

#pragma endregion

#pragma region Transform

    Transform transform_ = {
        {1.0f,1.0f,1.0f},   //scale
        {0.0f,0.0f,0.0f},   //rotate
        {0.0f,0.0f,0.0f}    //translate
    };

    // TransformationMatrix
    TransformationMatrix transformationMatrix{};

    // TransformationMatrixData
    TransformationMatrix* transformationData_ = nullptr;

#pragma endregion

#pragma region Material

    //uvTransform
    Transform uvTransform_{
        {1.0f,1.0f,1.0f},
        {0.0f,0.0f,0.0f},
        {0.0f,0.0f,0.0f}
    };

    //マテリアルデータ
    Material* materialData_ = nullptr;

#pragma endregion

#pragma region DirectionalLight

    DirectionalLight* directionalLightData_ = nullptr;

#pragma endregion

#pragma region Texture

    D3D12_GPU_DESCRIPTOR_HANDLE textureHandle_ = {};

#pragma endregion

#pragma region Buffer

    //頂点データバッファ
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

    //頂点インデックスバッファ
    D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

#pragma endregion

#pragma region ID3D12Resource

    // 頂点データ用定数バッファ
    ID3D12Resource* vertexResource_ = nullptr;
    //　頂点インデックス用定数バッファ
    ID3D12Resource* indexResource_ = nullptr;
    // 色用定数バッファ
    ID3D12Resource* materialResource_ = nullptr;
    // 拡縮回転移動行列用定数バッファ
    ID3D12Resource* transformationResource_ = nullptr;
    // 光用定数バッファ
    ID3D12Resource* directionalLightResource_ = nullptr;

#pragma endregion

#pragma region 外部参照

    static Microsoft::WRL::ComPtr<ID3D12Device> device_;

#pragma endregion

public: //メンバ関数
    //デストラクタ
    ~Resource() {
        if (vertexResource_) { vertexResource_->Release(); vertexResource_ = nullptr; }
        if (indexResource_) { indexResource_->Release(); indexResource_ = nullptr; }
        if (materialResource_) { materialResource_->Release(); materialResource_ = nullptr; }
        if (transformationResource_) { transformationResource_->Release(); transformationResource_ = nullptr; }
        if (directionalLightResource_) { directionalLightResource_->Release(); directionalLightResource_ = nullptr; }
        if (device_) { device_.Reset(); device_ = nullptr; }
    }

    //ID3D12Resourceを生成する
    void CreateResource() {
        vertexResource_ = CreateBufferResource(device_.Get(), sizeof(VertexData) * static_cast<size_t>(vertexDataList_.size()));
        indexResource_ = CreateBufferResource(device_.Get(), sizeof(uint32_t) * static_cast<size_t>(indexDataList_.size()));
        materialResource_ = CreateBufferResource(device_.Get(), sizeof(Material));
        transformationResource_ = CreateBufferResource(device_.Get(), sizeof(TransformationMatrix));
        directionalLightResource_ = CreateBufferResource(device_.Get(), sizeof(DirectionalLight));
    }

    //バッファへの書き込みを開放
    void Map() {
        vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
        indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
        materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
        transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationData_));
        directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
    }

    //バッファへの書き込みを閉鎖
    void UnMap() {
        vertexResource_->Unmap(0, nullptr);
        indexResource_->Unmap(0, nullptr);
        materialResource_->Unmap(0, nullptr);
        transformationResource_->Unmap(0, nullptr);
        directionalLightResource_->Unmap(0, nullptr);
    }
};

