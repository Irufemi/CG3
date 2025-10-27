#pragma once

#include <vector>
#include <memory>
#include <string>
#include <wrl.h>
#include <d3d12.h>
#include <cstdint>
#include <cassert>

#include "math/VertexData.h"
#include "function/Math.h"
#include "math/Transform.h"
#include "math/Material.h"
#include "math/Matrix4x4.h"
#include "math/Vector4.h"
#include "math/DirectionalLight.h"
#include "math/CameraForGPU.h"

class DirectXCommon;
class Camera;                       
class TextureManager;
class DrawManager;
class DescriptorAllocator; // 追加

class TetraRegion {
public:
    static void SetDirectXCommon(DirectXCommon* dx);
    static void SetTextureManager(TextureManager* tm);
    static void SetDrawManager(DrawManager* dm);
    static void SetSrvAllocator(DescriptorAllocator* alloc) { srvAllocator_ = alloc; } // 追加

    void Initialize(Camera* camera, const std::string& textureName = "resources/uvChecker.png");

    // 既存のTransformベース
    void AddInstance(const Transform& t);
    void AddInstance(const Vector3& center, float scale = 1.0f, const Vector3& rotate = {0,0,0});

    // 追加: World行列で直接インスタンスを追加（ドリル回転など用途）
    void AddInstanceWorld(const Matrix4x4& world, const Vector4& color = {1,1,1,1});

    void ClearInstances();
    void BuildInstanceBuffer(bool force = false);
    void Draw();

    // サイズ関連
    void SetEdge(float edge);
    float GetModelVertexRadius() const;
    float ComputeScaleFromVertexRadius(float worldVertexRadius) const;
    void AddInstanceByVertexRadius(const Vector3& center, float worldVertexRadius, const Vector3& rotate = {0,0,0});

    // DrawManager getters
    D3D12_VERTEX_BUFFER_VIEW&   GetVertexBufferView() { return vertexBufferView_; }
    D3D12_INDEX_BUFFER_VIEW&    GetIndexBufferView() { return indexBufferView_; }
    ID3D12Resource*             GetMaterialResource() { return materialResource_.Get(); }
    ID3D12Resource*             GetDirectionalLightResource() { return directionalLightResource_.Get(); }
    ID3D12Resource*             GetCameraResource() { return cameraResource_.Get(); }
    D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle() const { return textureHandle_; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetInstancingSrvHandleGPU() const { return instancingSrvGPU_; }
    UINT                        GetIndexCount() const { return indexCount_; }
    UINT                        GetInstanceCount() const {
        return static_cast<UINT>(instanceWorlds_.empty() ? instances_.size() : instanceWorlds_.size());
    }

private:
    struct InstanceData {
        Matrix4x4 WVP;
        Matrix4x4 World;
        Matrix4x4 WorldInverseTranspose;
        Vector4   color;
    };

    void BuildTetraMesh(std::vector<VertexData>& outVertices, std::vector<uint32_t>& outIndices);
    void CreateMeshBuffers(const std::vector<VertexData>& vertices, const std::vector<uint32_t>& indices);
    void CreateMaterialResources();
    void EnsureSharedTexture(const std::string& textureName);
    void EnsureLightAndCamera();
    void CreateOrResizeInstanceBuffer(uint32_t instanceCount);

private:
    static DirectXCommon* dx_;
    static TextureManager* textureManager_;
    static DrawManager* drawManager_;
    static DescriptorAllocator* srvAllocator_; // 追加

    Camera* camera_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
    D3D12_VERTEX_BUFFER_VIEW               vertexBufferView_{};
    UINT                                   vertexCount_ = 0;

    Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
    D3D12_INDEX_BUFFER_VIEW                indexBufferView_{};
    UINT                                   indexCount_ = 0;

    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_;

    D3D12_GPU_DESCRIPTOR_HANDLE textureHandle_{};

    Microsoft::WRL::ComPtr<ID3D12Resource> instanceBuffer_;
    D3D12_CPU_DESCRIPTOR_HANDLE            instancingSrvCPU_{};
    D3D12_GPU_DESCRIPTOR_HANDLE            instancingSrvGPU_{};
    uint32_t                               instancingSrvIndex_ = UINT32_MAX;

    // Transformベース
    std::vector<Transform> instances_;
    bool                   instanceDirty_ = false;

    // 追加: World行列ベース
    std::vector<Matrix4x4> instanceWorlds_;
    std::vector<Vector4>   instanceWorldColors_;

    // モデル辺長
    float edgeLength_ = 1.0f;
    bool  meshDirty_ = false;
};