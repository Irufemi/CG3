#pragma once

#include "math/SpotLight.h"
#include "engine/directX/DirectXCommon.h"
#include "wrl.h"
#include <d3d12.h>

class SpotLightClass{

private: // メンバ変数

    Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;
    SpotLight* data_ = nullptr;

    static DirectXCommon* dxCommon_;

public: // メンバ関数

    // 初期化
    void Initialize();

    // 更新
    void Debug();

    // PointLightの情報を渡す
    SpotLight* GetData() { return data_; }
    // PointLightの情報をセットする
    void SetData(SpotLight* info) { data_ = info; }

    ID3D12Resource* GetResource() { return resource_.Get(); }

    static void SetDxCommon(DirectXCommon* dxCommon) { dxCommon_ = dxCommon; }
};

