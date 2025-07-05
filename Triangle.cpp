#include "Triangle.h"
#include "Math.h"
#include "Function.h"

#include <algorithm>

void Triangle::Initialize(const Microsoft::WRL::ComPtr<ID3D12Device>& device, Camera* camera, TextureManager* textureManager, const std::string& textureName) {

    this->device_ = device;
    this->camera_ = camera;
    this->textureManager_ = textureManager;


    //左下
    vertexDataList_.push_back({ { -0.5f,-0.5f,0.0f,1.0f }, { 0.0f,1.0f } });
    //上
    vertexDataList_.push_back({ { 0.0f,0.5f,0.0f,1.0f  }, { 0.5f,0.0f } });
    //右下
    vertexDataList_.push_back({ { 0.5f,-0.5f,0.0f,1.0f }, { 1.0f,1.0f } });

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

    transformationResource_ = CreateBufferResource(device_.Get(), sizeof(TransformationMatrix));

    transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationData_));

    *transformationData_ = { wvpMatrix_,worldMatrix_ };

    transformationResource_->Unmap(0, nullptr);

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

void Triangle::Update(const char* triangleName,bool debug ) {

    if (debug) {

        #ifdef _DEBUG
        std::string name = std::string("Triangle: ") + triangleName;

        //ImGui

        //カメラウィンドウを作り出す
        ImGui::Begin(name.c_str());
        // scale
        ImGui::DragFloat3("scale", &transform_.scale.x, 0.1f);
        // rotate
        ImGui::DragFloat3("rotate", &transform_.rotate.x, 0.1f);
        // translate
        ImGui::DragFloat3("translate", &transform_.translate.x, 0.1f);
        //Color
        ImGui::ColorEdit4("color", &materialData_->color.x);

        std::vector<std::string> textureNames = textureManager_->GetTextureNames();
        std::sort(textureNames.begin(), textureNames.end());

        if (!textureNames.empty()) {
            selectedTextureIndex_ = std::clamp(selectedTextureIndex_, 0, (int)textureNames.size() - 1);
            if (ImGui::BeginCombo("Texture", textureNames[selectedTextureIndex_].c_str())) {
                for (int i = 0; i < textureNames.size(); ++i) {
                    bool isSelected = (i == selectedTextureIndex_);
                    if (ImGui::Selectable(textureNames[i].c_str(), isSelected)) {
                        selectedTextureIndex_ = i;
                        textureHandle_ = textureManager_->GetTextureHandle(textureNames[i]); // ★ここで更新
                    }
                }
                ImGui::EndCombo();
            }
        }
        else {
            ImGui::Text("No textures found.");
        }

        //入力終了
        ImGui::End();

        #endif // _DEBUG

    }
        transform_.rotate.y += 0.03f;
    

    vertexDataList_.clear();

    //左下
    vertexDataList_.push_back({ { -0.5f,-0.5f,0.0f,1.0f }, { 0.0f,1.0f } });
    //上
    vertexDataList_.push_back({ { 0.0f,0.5f,0.0f,1.0f  }, { 0.5f,0.0f } });
    //右下
    vertexDataList_.push_back({ { 0.5f,-0.5f,0.0f,1.0f }, { 1.0f,1.0f } });

    for (uint32_t i = 0; i < static_cast<uint32_t>(vertexDataList_.size()); ++i) {
        vertexDataList_[i].normal.x = vertexDataList_[i].position.x;
        vertexDataList_[i].normal.y = vertexDataList_[i].position.y;
        vertexDataList_[i].normal.z = vertexDataList_[i].position.z;
    }

    vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

    std::copy(vertexDataList_.begin(), vertexDataList_.end(), vertexData_);

    vertexResource_->Unmap(0, nullptr);

     materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

     materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
     materialData_->enableLighting = false;
     materialData_->uvTransform = Math::MakeIdentity4x4();

     materialResource_->Unmap(0, nullptr);

    worldMatrix_ = Math::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
    wvpMatrix_ = Math::Multiply(worldMatrix_, Math::Multiply(camera_->GetViewMatrix(), camera_->GetPerspectiveFovMatrix()));

    transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationData_));

    *transformationData_ = { wvpMatrix_,worldMatrix_ };

    transformationResource_->Unmap(0, nullptr);

    directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));

    directionalLightData_->direction = Math::Normalize(directionalLightData_->direction);

    directionalLightResource_->Unmap(0, nullptr);
}

