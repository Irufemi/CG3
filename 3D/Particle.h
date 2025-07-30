#pragma once

#include <memory>

#include "../source/D3D12ResourceUtil.h"


class Particle{
private: // メンバ変数

    // D3D12リソース
    std::unique_ptr<D3D12ResourceUtil> resource_ = nullptr;

public: // メンバ関数

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize();

    /// <summary>
    /// 更新
    /// </summary>
    void Update();

    /// <summary>
    /// 描画
    /// </summary>
    void Draw();
};

