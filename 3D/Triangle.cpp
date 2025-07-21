#include "Triangle.h"
#include "../Math.h"
#include "../function/Function.h"

#include <algorithm>

void Triangle::Initialize(const Microsoft::WRL::ComPtr<ID3D12Device>&device,Camera* camera, TextureManager* textureManager, const std::string& textureName) {

    this->camera_ = camera;
    this->textureManager_ = textureManager;

    // D3D12ResourceUtilを生成
    resource_ = std::make_unique<D3D12ResourceUtil>();

    //　左下
    resource_->vertexDataList_.push_back({ { -0.5f,-0.5f,0.0f,1.0f }, { 0.0f,1.0f } });
    //　上
    resource_->vertexDataList_.push_back({ { 0.0f,0.5f,0.0f,1.0f  }, { 0.5f,0.0f } });
    //　右下
    resource_->vertexDataList_.push_back({ { 0.5f,-0.5f,0.0f,1.0f }, { 1.0f,1.0f } });

    for (uint32_t i = 0; i < static_cast<uint32_t>(resource_->vertexDataList_.size()); ++i) {
        resource_->vertexDataList_[i].normal.x = resource_->vertexDataList_[i].position.x;
        resource_->vertexDataList_[i].normal.y = resource_->vertexDataList_[i].position.y;
        resource_->vertexDataList_[i].normal.z = resource_->vertexDataList_[i].position.z;
    }

    resource_->indexDataList_.push_back(0); //　左下 
    resource_->indexDataList_.push_back(1); //　上
    resource_->indexDataList_.push_back(2); //　右下

    // リソースのメモリを確保
    resource_->CreateResource(device.Get());

    // 書き込めるようにする
    resource_->Map();

    //頂点バッファ

    resource_->vertexBufferView_ = D3D12_VERTEX_BUFFER_VIEW{};

    resource_->vertexBufferView_.BufferLocation = resource_->vertexResource_->GetGPUVirtualAddress();
    resource_->vertexBufferView_.StrideInBytes = sizeof(VertexData);
    resource_->vertexBufferView_.SizeInBytes = sizeof(VertexData) * static_cast<UINT>(resource_->vertexDataList_.size());

    std::copy(resource_->vertexDataList_.begin(), resource_->vertexDataList_.end(), resource_->vertexData_);

    resource_->indexBufferView_ = D3D12_INDEX_BUFFER_VIEW{};
    //リソースの先頭のアドレスから使う
    resource_->indexBufferView_.BufferLocation = resource_->indexResource_->GetGPUVirtualAddress();
    //使用するリソースのサイズ
    resource_->indexBufferView_.SizeInBytes = sizeof(uint32_t) * static_cast<UINT>(resource_->indexDataList_.size());
    //インデックスはint32_tとする
    resource_->indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

    ///IndexResourceにデータを書き込む

    //インデックスリソースにデータを書き込む

    std::copy(resource_->indexDataList_.begin(), resource_->indexDataList_.end(), resource_->indexData_);

    //マテリアル

    resource_->materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
    resource_->materialData_->enableLighting = true;
    resource_->materialData_->uvTransform = Math::MakeIdentity4x4();

    //wvp

    resource_->transformationMatrix_.world = Math::MakeAffineMatrix(resource_->transform_.scale, resource_->transform_.rotate, resource_->transform_.translate);

    resource_->transformationMatrix_.WVP = Math::Multiply(resource_->transformationMatrix_.world, Math::Multiply(camera_->GetViewMatrix(), camera_->GetPerspectiveFovMatrix()));

    *resource_->transformationData_ = { resource_->transformationMatrix_.WVP,resource_->transformationMatrix_.world };

    auto textureNames = textureManager_->GetTextureNames();
    std::sort(textureNames.begin(), textureNames.end());
    if (!textureNames.empty()) {

        resource_->textureHandle_ = textureManager_->GetTextureHandle(textureName);

        // コンボボックス用に selectedIndex を初期化
        auto it = std::find(textureNames.begin(), textureNames.end(), textureName);
        if (it != textureNames.end()) {
            selectedTextureIndex_ = static_cast<int>(std::distance(textureNames.begin(), it));
        } else {
            selectedTextureIndex_ = 0;
        }

    }

    resource_->directionalLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
    resource_->directionalLightData_->direction = { 0.0f,-1.0f,0.0f, };
    resource_->directionalLightData_->intensity = 1.0f;

}

void Triangle::Update(const char* triangleName) {

#ifdef _DEBUG
    std::string name = std::string("Triangle: ") + triangleName;

    //ImGui

    //カメラウィンドウを作り出す
    ImGui::Begin(name.c_str());
    // scale
    ImGui::DragFloat3("scale", &resource_->transform_.scale.x, 0.1f);
    // rotate
    ImGui::DragFloat3("rotate", &resource_->transform_.rotate.x, 0.1f);
    // translate
    ImGui::DragFloat3("translate", &resource_->transform_.translate.x, 0.1f);
    //Color
    ImGui::ColorEdit4("color", &resource_->materialData_->color.x);

    std::vector<std::string> textureNames = textureManager_->GetTextureNames();
    std::sort(textureNames.begin(), textureNames.end());

    if (!textureNames.empty()) {
        selectedTextureIndex_ = std::clamp(selectedTextureIndex_, 0, (int)textureNames.size() - 1);
        if (ImGui::BeginCombo("Texture", textureNames[selectedTextureIndex_].c_str())) {
            for (int i = 0; i < textureNames.size(); ++i) {
                bool isSelected = (i == selectedTextureIndex_);
                if (ImGui::Selectable(textureNames[i].c_str(), isSelected)) {
                    selectedTextureIndex_ = i;
                    resource_->textureHandle_ = textureManager_->GetTextureHandle(textureNames[i]); // ★ここで更新
                }
            }
            ImGui::EndCombo();
        }
    } else {
        ImGui::Text("No textures found.");
    }

    //入力終了
    ImGui::End();

#endif // _DEBUG

    resource_->transform_.rotate.y += 0.03f;


    resource_->vertexDataList_.clear();

    //左下
    resource_->vertexDataList_.push_back({ { -0.5f,-0.5f,0.0f,1.0f }, { 0.0f,1.0f } });
    //上
    resource_->vertexDataList_.push_back({ { 0.0f,0.5f,0.0f,1.0f  }, { 0.5f,0.0f } });
    //右下
    resource_->vertexDataList_.push_back({ { 0.5f,-0.5f,0.0f,1.0f }, { 1.0f,1.0f } });

    for (uint32_t i = 0; i < static_cast<uint32_t>(resource_->vertexDataList_.size()); ++i) {
        resource_->vertexDataList_[i].normal.x = resource_->vertexDataList_[i].position.x;
        resource_->vertexDataList_[i].normal.y = resource_->vertexDataList_[i].position.y;
        resource_->vertexDataList_[i].normal.z = resource_->vertexDataList_[i].position.z;
    }

    std::copy(resource_->vertexDataList_.begin(), resource_->vertexDataList_.end(), resource_->vertexData_);

    resource_->materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
    resource_->materialData_->enableLighting = false;
    resource_->materialData_->uvTransform = Math::MakeIdentity4x4();

    resource_->transformationMatrix_.world = Math::MakeAffineMatrix(resource_->transform_.scale, resource_->transform_.rotate, resource_->transform_.translate);
    resource_->transformationMatrix_.WVP = Math::Multiply(resource_->transformationMatrix_.world, Math::Multiply(camera_->GetViewMatrix(), camera_->GetPerspectiveFovMatrix()));

    *resource_->transformationData_ = { resource_->transformationMatrix_.WVP,resource_->transformationMatrix_.world };

    resource_->directionalLightData_->direction = Math::Normalize(resource_->directionalLightData_->direction);
}

