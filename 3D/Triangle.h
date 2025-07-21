#pragma once

#include <vector>
#include <array>
#include <d3d12.h>

#include "../source/D3D12ResourceUtil.h"
#include "../camera/Camera.h"
#include "../manager/TextureManager.h"
#include <wrl.h>
#include <memory>


class Triangle {
protected: //メンバ変数
    
    std::unique_ptr<D3D12ResourceUtil> resource_ = nullptr;

    bool isRotateY_ = true;

    int selectedTextureIndex_ = 0;

    // ポインタ参照

    Camera* camera_ = nullptr;

    TextureManager* textureManager_ = nullptr;

public: //メンバ関数
    //デストラクタ
    ~Triangle() = default;

    //初期化
    void Initialize(const Microsoft::WRL::ComPtr<ID3D12Device>& device, Camera* camera, TextureManager* textureManager, const std::string& textureName = "uvChecker.png");
    //更新
    void Update(const char* triangleName = "");

    //ゲッター
    D3D12ResourceUtil* GetD3D12Resource() { return this->resource_.get(); }
    
};

