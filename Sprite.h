#pragma once

#include <d3d12.h>
#include <vector>
#include <cstdint>
#include "manager/TextureManager.h"
#include "D3D12ResourceUtil.h"
#include "Camera.h"
#include <wrl.h>
#include <memory>

class Sprite {
protected:

    std::unique_ptr<D3D12ResourceUtil> resource_ = nullptr;

    bool isRotateY_ = true;

    int selectedTextureIndex_ = 0;

    Camera* camera_ = nullptr;

    TextureManager* textureManager_ = nullptr;

public: //メンバ関数
    //デストラクタ
    ~Sprite() = default;

    //初期化
    void Initialize(const Microsoft::WRL::ComPtr<ID3D12Device>& device, Camera* camera, TextureManager* textureManager, const std::string& textureName = "uvChecker.png");
    //更新
    void Update();

    // ゲッター
    D3D12ResourceUtil* GetD3D12Resource() { return this->resource_.get(); }

};