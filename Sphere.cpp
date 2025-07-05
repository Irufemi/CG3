#include "Sphere.h"

#include <cmath>
#include "externals/imgui/imgui.h"

#include "Function.h"
#include "Math.h"
#include <string>
#include <algorithm>

//初期化
void Sphere::Initialize(const Microsoft::WRL::ComPtr<ID3D12Device>& device, Camera* camera, TextureManager* textureManager, const std::string& textureName) {

    this->camera_ = camera;
    this->device_ = device;
    this->textureManager_ = textureManager;

    //上と下で頂点は重なるがtexcoordがおかしくなるので必要

    //緯度の方向に分割
    for (uint32_t latIndex = 0; latIndex <= kSubdivision_; ++latIndex) {
        float lat = -pi_ / 2.0f + kLatEvery_ * latIndex; //θ

        //経度の方向に分割しながら線を描く
        for (uint32_t lonIndex = 0; lonIndex <= kSubdivision_; ++lonIndex) {

            float lon = lonIndex * kLonEvery_; //φ

            //頂点にデータを入力する。基準点a

             //頂点リソースにデータを書き込む

            vertexDataList_.push_back( //左下
                {
                    {
                        std::cos(lat) * std::cos(lon),
                        std::sin(lat),
                        std::cos(lat) * std::sin(lon),
                        1.0f
                    },
                    {
                        static_cast<float>(lonIndex) / static_cast<float>(kSubdivision_),
                        1.0f - static_cast<float>(latIndex) / static_cast<float>(kSubdivision_)
                    }
                }
            );

        }
    }

    for (uint32_t i = 0; i < static_cast<uint32_t>(vertexDataList_.size()); ++i) {
        vertexDataList_[i].normal.x = vertexDataList_[i].position.x;
        vertexDataList_[i].normal.y = vertexDataList_[i].position.y;
        vertexDataList_[i].normal.z = vertexDataList_[i].position.z;
    }

    //頂点バッファ
    vertexResource_ = CreateBufferResource(device_.Get(), sizeof(VertexData) * static_cast<size_t>(vertexDataList_.size()));

    vertexBufferView_ = D3D12_VERTEX_BUFFER_VIEW{};

    vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    vertexBufferView_.StrideInBytes = sizeof(VertexData);
    vertexBufferView_.SizeInBytes = sizeof(VertexData) * static_cast<UINT>(vertexDataList_.size());

    vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

    std::copy(vertexDataList_.begin(), vertexDataList_.end(), vertexData_);

    vertexResource_->Unmap(0, nullptr);

    /*頂点インデックス*/

    for (uint32_t latIndex = 0; latIndex < kSubdivision_; ++latIndex) {
        for (uint32_t lonIndex = 0; lonIndex < kSubdivision_; ++lonIndex) {
            indexDataList_.push_back((kSubdivision_ + 1) * latIndex + lonIndex);
            indexDataList_.push_back((kSubdivision_ + 1) * (latIndex + 1) + lonIndex);//左上
            indexDataList_.push_back((kSubdivision_ + 1) * latIndex + (lonIndex + 1));//右下
            indexDataList_.push_back((kSubdivision_ + 1) * (latIndex + 1) + lonIndex);//左上
            indexDataList_.push_back((kSubdivision_ + 1) * (latIndex + 1) + (lonIndex + 1));//右上
            indexDataList_.push_back((kSubdivision_ + 1) * latIndex + (lonIndex + 1));//右下
        }
    }

    ///Index用のあれやこれやを作る

    indexResource_ = CreateBufferResource(device_.Get(), sizeof(uint32_t) * static_cast<size_t>(indexDataList_.size()));

    ///Index用のあれやこれやを作る

    indexBufferView_ = D3D12_INDEX_BUFFER_VIEW{};
    //リソースの先頭のアドレスから使う
    indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
    //使用するリソースのサイズ
    indexBufferView_.SizeInBytes = sizeof(uint32_t) * static_cast<UINT>(indexDataList_.size());
    //インデックスはint32_tとする
    indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

    ///IndexResourceにデータを書き込む

    //インデックスリソースにデータを書き込む

    indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

    std::copy(indexDataList_.begin(), indexDataList_.end(), indexData_);

    indexResource_->Unmap(0, nullptr);

    //マテリアル
    materialResource_ = CreateBufferResource(device_.Get(), sizeof(Material));

    materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

    materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
    materialData_->enableLighting = false;
    materialData_->uvTransform = Math::MakeIdentity4x4();

    materialResource_->Unmap(0, nullptr);

    //wvp

    worldMatrix_ = Math::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);

    wvpMatrix_ = Math::Multiply(worldMatrix_, Math::Multiply(camera_->GetViewMatrix(), camera_->GetPerspectiveFovMatrix()));

    wvpResource_ = CreateBufferResource(device_.Get(), sizeof(TransformationMatrix));

    wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));

    *wvpData_ = { wvpMatrix_,worldMatrix_ };

    wvpResource_->Unmap(0, nullptr);

    auto textureNames = textureManager_->GetTextureNames();
    std::sort(textureNames.begin(), textureNames.end());
    if (!textureNames.empty()) {

        textureHandle_ = textureManager_->GetTextureHandle(textureName);

        // コンボボックス用に selectedIndex を初期化
        auto it = std::find(textureNames.begin(), textureNames.end(), textureName);
        if (it != textureNames.end()) {
            selectedTextureIndex_ = static_cast<int>(std::distance(textureNames.begin(), it));
        }
        else {
            selectedTextureIndex_ = 0;
        }

    }

    directionalLightResource_ = CreateBufferResource(device_.Get(), sizeof(DirectionalLight));

    directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));

    directionalLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
    directionalLightData_->direction = { 0.0f,-1.0f,0.0f, };
    directionalLightData_->intensity = 1.0f;

    directionalLightResource_->Unmap(0, nullptr);
}

void Sphere::Update(const char* sphereName) {


#ifdef _DEBUG

    std::string name = std::string("Sphere: ") + sphereName;

    ImGui::Begin(name.c_str());
    ImGui::DragFloat3("center", &transform_.translate.x, 0.05f);
    ImGui::DragFloat3("scale", &transform_.scale.x, 0.05f);
    ImGui::DragFloat3("rotate", &transform_.rotate.x, 0.05f);
    ImGui::DragFloat3("translate", &transform_.translate.x, 0.05f);
    ImGui::ColorEdit4("spriteColor", &materialData_->color.x);
    ImGui::SliderInt("enableLighting", &materialData_->enableLighting, 0, 1);



    std::vector<std::string> textureNames = textureManager_->GetTextureNames();
    std::sort(textureNames.begin(), textureNames.end());

    if (!textureNames.empty()) {
        selectedTextureIndex_ = std::clamp(selectedTextureIndex_, 0, (int)textureNames.size() - 1);
        if (ImGui::BeginCombo("Texture", textureNames[selectedTextureIndex_].c_str())) {
            for (int i = 0; i < textureNames.size(); ++i) {
                bool isSelected = (i == selectedTextureIndex_);
                if (ImGui::Selectable(textureNames[i].c_str(), isSelected)) {
                    selectedTextureIndex_ = i;
                    textureHandle_ = textureManager_->GetTextureHandle(textureNames[i]); 
                }
            }
            ImGui::EndCombo();
        }
    }
    else {
        ImGui::Text("No textures found.");
    }

    ImGui::ColorEdit4("lightColor", &directionalLightData_->color.x);
    ImGui::DragFloat3("lightDirection", &directionalLightData_->direction.x, 0.01f);
    ImGui::DragFloat("intensity", &directionalLightData_->intensity, 0.01f);

    ImGui::End();

#endif // _DEBUG

    worldMatrix_ = Math::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);

    wvpMatrix_ = Math::Multiply(worldMatrix_, Math::Multiply(camera_->GetViewMatrix(), camera_->GetPerspectiveFovMatrix()));

    wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));

    *wvpData_ = { wvpMatrix_,worldMatrix_ };

    wvpResource_->Unmap(0, nullptr);

    directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));

    /*directionalLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
    directionalLightData_->intensity = 1.0f;*/

    directionalLightData_->direction = Math::Normalize(directionalLightData_->direction);

    directionalLightResource_->Unmap(0, nullptr);
    
}

void Sphere::Release() {

    delete vertexData_;
    delete materialData_;
    delete wvpData_;
    vertexResource_->Release();
    wvpResource_->Release();
    materialResource_->Release();
}