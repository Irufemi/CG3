#include "Obj.h"

#include "../function/Function.h"
#include "../Math.h"

void Obj::Initialize(const Microsoft::WRL::ComPtr<ID3D12Device>& device, Camera* camera, ID3D12DescriptorHeap* srvDescriptorHeap, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList, const std::string& filename) {

    this->camera_ = camera;
    this->device_ = device;

    /*objファイルを読んでみよう*/

    ///ModelDataを使う

    modelData_ = LoadObjFile("resources", filename);

    //頂点リソースを作る
    vertexResource_ = CreateBufferResource(device.Get(), sizeof(VertexData) * modelData_.vertices.size());
    //頂点バッファ－ビューを作成する
    vertexBufferView_ = D3D12_VERTEX_BUFFER_VIEW{};
    vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress(); //リソースの先頭のアドレスから使う
    vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());//使用するリソースのサイズは頂点のサイズ
    vertexBufferView_.StrideInBytes = sizeof(VertexData);//1頂点あたりのサイズ

    //頂点リソースにデータを書き込む
    vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_)); //書き込むためのアドレスを取得
    std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size()); //頂点データをリソースにコピー

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

    wvpResource_ = CreateBufferResource(device_.Get(), sizeof(TransformationMatrix));

    wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationData_));

    *transformationData_ = { wvpMatrix_,worldMatrix_ };

    wvpResource_->Unmap(0, nullptr);

    texture.Initialize(modelData_.material.textureFilePath, device_.Get(), srvDescriptorHeap, commandList);

    directionalLightResource_ = CreateBufferResource(device_.Get(), sizeof(DirectionalLight));

    directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));

    directionalLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
    directionalLightData_->direction = { 0.0f,-1.0f,0.0f, };
    directionalLightData_->intensity = 1.0f;

    directionalLightResource_->Unmap(0, nullptr);

}

void Obj::Update(const char* objName) {
#ifdef _DEBUG

    std::string name = std::string("Obj: ") + objName;

    ImGui::Begin(name.c_str());
    ImGui::DragFloat3("center", &transform_.translate.x, 0.05f);
    ImGui::DragFloat3("scale", &transform_.scale.x, 0.05f);
    ImGui::DragFloat3("rotate", &transform_.rotate.x, 0.05f);
    ImGui::DragFloat3("translate", &transform_.translate.x, 0.05f);
    ImGui::ColorEdit4("spriteColor", &materialData_->color.x);
    ImGui::SliderInt("enableLighting", &materialData_->enableLighting, 0, 1);

    ImGui::ColorEdit4("lightColor", &directionalLightData_->color.x);
    ImGui::DragFloat3("lightDirection", &directionalLightData_->direction.x, 0.01f);
    ImGui::DragFloat("intensity", &directionalLightData_->intensity, 0.01f);

    ImGui::End();

#endif // _DEBUG

    worldMatrix_ = Math::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);

    wvpMatrix_ = Math::Multiply(worldMatrix_, Math::Multiply(camera_->GetViewMatrix(), camera_->GetPerspectiveFovMatrix()));

    wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationData_));

    *transformationData_ = { wvpMatrix_,worldMatrix_ };

    wvpResource_->Unmap(0, nullptr);

    directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));

    /*directionalLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
    directionalLightData_->direction = { 0.0f,-1.0f,0.0f, };
    directionalLightData_->intensity = 1.0f;*/
    directionalLightData_->direction = Math::Normalize(directionalLightData_->direction);

    directionalLightData_->direction = Math::Normalize(directionalLightData_->direction);

    directionalLightResource_->Unmap(0, nullptr);
}


