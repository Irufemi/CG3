#pragma once

#include <d3d12.h>
#include <string>
#include "../camera/Camera.h"
#include "../source/D3D12ResourceUtil.h"
#include <wrl.h>
#include <memory>

// 前方宣言
class TextureManager;
class DrawManager;
class DebugUI;

//==========================
// objが配布されているサイト
// https://quaternius.com/
// 使用する場合はライセンスがCCOのものを利用する
// https://creativecommons.org/publicdomain/zero/1.0/deed.ja
//==========================

class ObjClass {
protected: //メンバ変数

    ObjModel objModel_;

    std::vector<std::unique_ptr<Texture>> textures_;

    std::vector<std::unique_ptr<D3D12ResourceUtil>> resources_;

#pragma region 外部参照

    Camera* camera_ = nullptr;

    static TextureManager* textureManager_;

    static DrawManager* drawManager_;

    static DebugUI* ui_;

#pragma endregion


public: //メンバ関数

    //デストラクタ
    ~ObjClass() = default;

    //初期化
    void Initialize(Camera* camera, const std::string& filename = "plane.obj");

    void Update(const char* objName = " ");

    void Draw();

    static void SetTextureManager(TextureManager* texM) { textureManager_ = texM; }
    static void SetDrawManager(DrawManager* drawM) { drawManager_ = drawM; }
    static void SetDebugUI(DebugUI* ui) { ui_ = ui; }

};

