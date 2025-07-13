#pragma once

#include "math/Transform.h"
#include "math/Material.h"
#include "math/TransformationMatrix.h"
#include "math/DirectionalLight.h"
#include "math/ModelData.h"
#include "math/VertexData.h"
#include "math/Matrix4x4.h"
#include <d3d12.h>
#include <string>
#include "Camera.h"
#include "Texture.h"
#include <wrl.h>

//==========================
// objが配布されているサイト
// https://quaternius.com/
// 使用する場合はライセンスがCCOのものを利用する
// https://creativecommons.org/publicdomain/zero/1.0/deed.ja
//==========================

class Obj {
protected: //メンバ変数

    Transform transform_ = {
        {1.0f,1.0f,1.0f},   //scale
        {0.0f,0.0f,0.0f},   //rotate
        {0.0f,0.0f,0.0f}    //translate
    };

    //頂点バッファ
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

    ModelData modelData_{};
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
    ID3D12Resource* wvpResource_ = nullptr;
    // 色用定数バッファ
    ID3D12Resource* materialResource_ = nullptr;

    ID3D12Resource* directionalLightResource_ = nullptr;

    Texture texture{};

#pragma region 外部参照

    Camera* camera_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;

#pragma endregion


public: //メンバ関数

    //デストラクタ
    ~Obj() {
        if (vertexResource_) { vertexResource_->Release(); vertexResource_ = nullptr; }
        if (wvpResource_) { wvpResource_->Release(); wvpResource_ = nullptr; }
        if (materialResource_) { materialResource_->Release(); materialResource_ = nullptr; }
        if (directionalLightResource_) { directionalLightResource_->Release(); directionalLightResource_ = nullptr; }
        if (device_) { device_.Reset(); device_ = nullptr; }
    }

    //初期化
    void Initialize(const Microsoft::WRL::ComPtr<ID3D12Device>& device, Camera* camera, ID3D12DescriptorHeap* srvDescriptorHeap, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList, const std::string& filename = "plane.obj");

    void Update(const char* objName = " ");

    D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() { return vertexBufferView_; }
    ID3D12Resource* GetWvpResource() { return wvpResource_; }
    ID3D12Resource* GetMaterialResource() { return materialResource_; }
    ID3D12Resource* GetDirectionalLightResource() { return directionalLightResource_; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU() { return texture.GetTextureSrvHandleGPU(); }
    UINT GetModelDataVerticesSize() { return static_cast<UINT>(modelData_.vertices.size()); }
};

