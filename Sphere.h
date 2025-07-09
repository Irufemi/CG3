#pragma once

#include <cstdint>

#include "Camera.h"
#include "manager/TextureManager.h"
#include <vector>
#include <d3d12.h>
#include <wrl.h>
#include "D3D12ResourceUtil.h"



class Sphere {
protected: //メンバ変数

    const float pi_ = 3.141592654f;

    const uint32_t kSubdivision_ = 16;

    //経度分割1つ分の角度 Φd

    const float kLonEvery_ = pi_ * 2.0f / static_cast<float>(kSubdivision_);

    //井戸分割つ分の角度 θd
    const float kLatEvery_ = pi_ / static_cast<float>(kSubdivision_);

    bool isRotateY_ = true;

    // D3D12リソース

    std::unique_ptr<D3D12ResourceUtil> resource_ = nullptr;

    int selectedTextureIndex_ = 0;

    // ポインタ参照

    TextureManager* textureManager_ = nullptr;

    Camera* camera_ = nullptr;

public: //メンバ関数
    // コンストラクタ
    Sphere() {};

    // デストラクタ
    ~Sphere() {
        if (resource_) { resource_.release(); }
    }

    // 初期化
    void Initialize(Camera* camera, TextureManager* textureManager, const std::string& textureName = "uvChecker.png");

    // 更新
    void Update(const char* sphereName = " ");

    D3D12ResourceUtil* GetD3D12Resource() { return this->resource_.get(); }
    void AddRotateY(float value) { this->resource_->transform_.rotate.y += value; }
};

