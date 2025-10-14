#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <cstdint>
#include <array>
#include <wrl.h>

// 前方宣言
class DirectXCommon;
class Sprite;
class SphereClass;
class ObjClass;
class ParticleClass;
class D3D12ResourceUtil;

//描画のCommandListを積む順番
// Viewport → RootSignature → Pipeline → Topology → Buffers → CBV → SRV → Draw

class DrawManager{
private:

    DirectXCommon* dxCommon_ = nullptr;

public: //メンバ関数

    void Initialize(DirectXCommon* dx) { dxCommon_ = dx; }
    void Finalize() { dxCommon_ = nullptr; }

    // 追加（保持はしないで即時バインド）
    void BindPSO(ID3D12PipelineState* pso);

    void PreDraw(
        std::array<float, 4> clearColor = { 0.1f, 0.25f, 0.5f, 1.0f },
        float clearDepth = 1.0f,
        uint8_t clearStencil = 0
    );

    void PostDraw(
    );

    void DrawTriangle(
        D3D12_VERTEX_BUFFER_VIEW& vertexBufferView,
        ID3D12Resource* materialResource,
        ID3D12Resource* wvpResource,
        ID3D12Resource* directionalLightResource,
        D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU
    );

    void DrawSprite(Sprite* sprite);

    void DrawSphere(SphereClass* sphere);

    void DrawParticle(ParticleClass* resource);

    void DrawByIndex(D3D12ResourceUtil* resource);

    void DrawByVertex(D3D12ResourceUtil* resource);
};