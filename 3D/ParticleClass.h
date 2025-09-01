#pragma once

#include "../source/D3D12ResourceUtil.h"
#include "../camera/Camera.h"
#include "../manager/TextureManager.h"
#include "../manager/DebugUI.h"
#include <wrl.h>
#include <memory>
#include <cstdint>


class ParticleClass{
private: // メンバ変数

    static inline const uint32_t kNumInstance = 10; // インスタンス数

    Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource = nullptr;

    TransformationMatrix* instancingData = nullptr;

    D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU{};

    D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU{};

    Transform transforms[kNumInstance];

    std::unique_ptr<D3D12ResourceUtilParticle> resource_ = nullptr;

    // パーティクルの粒子の数
    int32_t instanceCount_ = 10;

    int selectedTextureIndex_ = 0;

    // ポインタ参照

    Camera* camera_ = nullptr;

    TextureManager* textureManager_ = nullptr;

    DebugUI* ui_ = nullptr;

public: // メンバ関数

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& srvDescriptorHeap, Camera* camera, TextureManager* textureManager, DebugUI* ui, const std::string& textureName = "uvChecker.png");

    /// <summary>
    /// 更新
    /// </summary>
    void Update(const char* particleName = "");

    /// <summary>
    /// 描画
    /// </summary>
    void Draw();

    //ゲッター
    D3D12ResourceUtilParticle* GetD3D12Resource() { return this->resource_.get(); }
    int32_t GetInstanceCount() const { return this->kNumInstance; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetInstancingSrvHandleGPU() const { return instancingSrvHandleGPU; }
};

