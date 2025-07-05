#pragma once

#include <vector>
#include <array>
#include <d3d12.h>

#include "Matrix4x4.h"
#include "TransformationMatrix.h"
#include "DirectionalLight.h"
#include "Material.h"
#include "Transform.h"
#include "VertexData.h"
#include "Camera.h"
#include "TextureManager.h"
#include <wrl.h>


class Triangle {
protected: //メンバ変数
    Transform transform_ = {
        {1.0f,1.0f,1.0f},   //scale
        {0.0f,0.0f,0.0f},   //rotate
        {0.0f,0.0f,0.0f}    //translate
    };

    // 頂点データ
    std::vector<VertexData> vertexDataList_{};
    //頂点バッファ
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
    //頂点リソース
    VertexData* vertexData_ = nullptr;
    //マテリアルデータ
    Material* materialData_ = nullptr;

    Transform uvTransform_{
        {1.0f,1.0f,1.0f},
        {0.0f,0.0f,0.0f},
        {0.0f,0.0f,0.0f}
    };

    // ワールド行列
    Matrix4x4 worldMatrix_{};

    Matrix4x4 wvpMatrix_{};

    TransformationMatrix* transformationData_ = nullptr;

    DirectionalLight* directionalLightData_ = nullptr;

    ID3D12Resource* vertexResource_ = nullptr;
    // 行列用定数バッファ
    ID3D12Resource* transformationResource_ = nullptr;
    // 色用定数バッファ
    ID3D12Resource* materialResource_ = nullptr;

    ID3D12Resource* directionalLightResource_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;

    Camera* camera_ = nullptr;

    TextureManager* textureManager_ = nullptr;

    bool isRotateY_ = true;

    D3D12_GPU_DESCRIPTOR_HANDLE textureHandle_ = {};

    int selectedTextureIndex_ = 0;

public: //メンバ関数
    //デストラクタ
    ~Triangle() {
        if (vertexResource_) { vertexResource_->Release(); vertexResource_ = nullptr; }
        if (transformationResource_) { transformationResource_->Release(); transformationResource_ = nullptr; }
        if (materialResource_) { materialResource_->Release(); materialResource_ = nullptr; }
        if (device_) { device_.Reset();}
    }

    //初期化
    void Initialize(const Microsoft::WRL::ComPtr<ID3D12Device>& device, Camera* camera, TextureManager* textureManager, const std::string& textureName = "uvChecker.png");
    //更新
    void Update(const char* triangleName, bool debug = true);

    //ゲッター

    D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() { return vertexBufferView_; }
    ID3D12Resource* GetWvpResource() { return transformationResource_; }
    ID3D12Resource* GetMaterialResource() { return materialResource_; }
    ID3D12Resource* GetDirectionalLightResource() { return directionalLightResource_; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle() { return textureHandle_; }
    void SetRotateY(float value) { transform_.rotate.y = value; }
    void SetTransform(Transform transform1) { this->transform_ = transform1; }
    void SetColor(Material  material) { *this->materialData_ = material; }
    void AddRotateY(float value) { transform_.rotate.y += value; }

};

