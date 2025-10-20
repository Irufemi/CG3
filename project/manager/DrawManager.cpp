#include "DrawManager.h"

#include<Windows.h>
#include <cassert>

#include <dxgidebug.h>
#include "../3D/SphereClass.h"
#include "../2D/Sprite.h"
#include "../3D/ObjClass.h"
#include "../3D/TriangleClass.h"
#include "../3D/ParticleClass.h"

#include "../source/D3D12ResourceUtil.h"
#include "engine/directX/DirectXCommon.h"

void DrawManager::BindPSO(ID3D12PipelineState* pso) {
    if (!pso) { return; }
    dxCommon_->GetCommandList()->SetPipelineState(pso);
}

void DrawManager::PreDraw(std::array<float, 4> clearColor, float clearDepth, uint8_t clearStencil) {

    // バックバッファとRTV/DSVの取得
    const UINT backIdx = dxCommon_->GetSwapChain()->GetCurrentBackBufferIndex();
    ID3D12Resource* backBuffer = dxCommon_->GetSwapChainResources(backIdx);
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = dxCommon_->GetRtvHandles(backIdx);
    auto* dsvHeap = dxCommon_->GetDsvDescriptorHeap();
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();

    /*完璧な画面クリアを目指して*/

    ///TransitionBarrierを張るコード

    //TransitionBarrierの設定
    D3D12_RESOURCE_BARRIER barrier{};
    //今回のバリアはTransition
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    //Noneにしておく
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    //バリアを張る対象のリソース。現在のバックバッファに対して行う
    barrier.Transition.pResource = backBuffer;
    // リソースバリアの Subresource を D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES にして明示
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    //遷移前(現在)のResourceState
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    //遷移後のResourceState
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    //TransitionBarrierを張る
    dxCommon_->GetCommandList()->ResourceBarrier(1, &barrier);

    /*画面の色を変えよう*/

    ///コマンドを積み込んで確定させる

    //描画先のRTVを設定する
    dxCommon_->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, nullptr);
    //指定した色で画面全体をクリアする
    dxCommon_->GetCommandList()->ClearRenderTargetView(rtvHandle, clearColor.data(), 0, nullptr);

    /*前後関係を正しくしよう*/

    ///DSVを設定する

    //描画先のRTVとDSVを設定する
    dxCommon_->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

    //指定した深度で画面全体をクリアする
    dxCommon_->GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, clearDepth, clearStencil, 0, nullptr);

    // フレーム共通のビューポート/シザーを一度だけ設定
    auto& viewport = dxCommon_->GetViewport();
    auto& scissorRect = dxCommon_->GetScissorRect();
    dxCommon_->GetCommandList()->RSSetViewports(1, &viewport);
    dxCommon_->GetCommandList()->RSSetScissorRects(1, &scissorRect);

    /*開発のUIを出そう*/

    ///ImGuiを描画する

    //描画用のDescriptorHeapの設定
    ID3D12DescriptorHeap* descriptorHeaps[] = { dxCommon_->GetSrvDescriptorHeap() };
    dxCommon_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
}

void DrawManager::PostDraw() {

    const UINT backIdx = dxCommon_->GetSwapChain()->GetCurrentBackBufferIndex();
    ID3D12Resource* backBuffer = dxCommon_->GetSwapChainResources(backIdx);

    D3D12_RESOURCE_BARRIER barrier{};

    /*完璧な画面クリアを目指して*/

    //今回のバリアはTransition
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    //Noneにしておく
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    //バリアを張る対象のリソース。現在のバックバッファに対して行う
    barrier.Transition.pResource = backBuffer;
    // リソースバリアの Subresource を D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES にして明示
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    //画面に描く処理はすべて終わり、画面に映すので、状態を遷移
    //今回はRenderTargetからPresentにする
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    //TransitionBarrierを張る                                                                                                                                                                                                                                                                                                                                                                                                                                                                rrierを張る
    dxCommon_->GetCommandList()->ResourceBarrier(1, &barrier);

    /*画面の色を変えよう*/

    ///コマンドを積み込んで確定させる

    //コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
    HRESULT hr = dxCommon_->GetCommandList()->Close();
    assert(SUCCEEDED(hr));

    ///コマンドをキックする

    //GPUにコマンドリストの実行を行わせる
    ID3D12CommandList* commandLists[] = { dxCommon_->GetCommandList() };
    dxCommon_->GetCommandQueue()->ExecuteCommandLists(1, commandLists);
    //GPUとOSに画面の交換を行うよう通知する
    dxCommon_->GetSwapChain()->Present(1, 0);

    /*完璧な画面クリアを目指して*/

    ///GPUにSignal(シグナル)を送る

    //Fenceの値を更新
    uint64_t& fenceValue = dxCommon_->GetFenceValue();
    fenceValue++;
    //GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
    dxCommon_->GetCommandQueue()->Signal(dxCommon_->GetFence(), fenceValue);

    ///Fenceの値を確認してGPUを待つ

    //Fenceの値が指定したSignal値にたどり着いているか確認する
    //GetCompletedValueの初期値はFence作成時に渡した初期値
    if (dxCommon_->GetFence()->GetCompletedValue() < fenceValue) {
        //指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
        dxCommon_->GetFence()->SetEventOnCompletion(fenceValue, dxCommon_->GetFenceEvent());
        //イベント待つ
        WaitForSingleObject(dxCommon_->GetFenceEvent(), INFINITE);
    }

    dxCommon_->UpdateFixFPS();

    /*画面の色を変えよう*/

    ///コマンドを積み込んで確定させる

    //次のフレーム用のコマンドリストを準備
    hr = dxCommon_->GetCommandAllocator()->Reset();
    assert(SUCCEEDED(hr));
    hr = dxCommon_->GetCommandList()->Reset(dxCommon_->GetCommandAllocator(), nullptr);
    assert(SUCCEEDED(hr));

}

void DrawManager::EnsurePointLightResource() {
    if (pointLight_) return;

    pointLight_->Initialize();

}

void DrawManager::EnsureSpotLightResource() {
    if (spotLight_) return;

    spotLight_->Initialize();

}

void DrawManager::DrawTriangle(
    D3D12_VERTEX_BUFFER_VIEW& vertexBufferView,
    ID3D12Resource* materialResource,
    ID3D12Resource* wvpResource,
    ID3D12Resource* directionalLightResource,
    D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU
) {

    /*三角形を表示しよう*/
    //RootSignatureを設定。PSOに設定しているけど別途指定が必要
    dxCommon_->GetCommandList()->SetGraphicsRootSignature(dxCommon_->GetRootSignature());
    dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView); // VBVを設定
    //形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
    dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    /*三角形の色を変えよう*/

    ///CBVを設定する

    //マテリアルCBufferの場所を設定(ここでの第一引数の0はRootParameter配列の0番目であり、registerの0ではない)
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());

    /*三角形を動かそう*/

    //wvp用のCbufferの場所を設定(今回はRootParameter[1]に対してCBVの設定を行っている)
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());


    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

    /*テクスチャを貼ろう*/

    ///DescriptorTableを設定する

    //SRVのDescriptorTableの先頭を設定。2はRootParameter[2]である。
    dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);

    EnsurePointLightResource();
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(6, pointLight_->GetResource()->GetGPUVirtualAddress());

    EnsureSpotLightResource();
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(7, spotLight_->GetResource()->GetGPUVirtualAddress());

    /*三角形を表示しよう*/

    //描画！(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
    dxCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);

}

void DrawManager::DrawSprite(Sprite* sprite) {

    // 2. パイプラインの基本構成（RootSignature, PSO）

    //RootSignatureを設定。PSOに設定しているけど別途指定が必要
    dxCommon_->GetCommandList()->SetGraphicsRootSignature(dxCommon_->GetRootSignature());

    // 3. バッファ設定（VBV、IBV、Topology）

    //形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
    dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //VBVを設定
    dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &sprite->GetD3D12Resource()->vertexBufferView_);
    //IBVを設定
    dxCommon_->GetCommandList()->IASetIndexBuffer(&sprite->GetD3D12Resource()->indexBufferView_);

    // 4. 定数バッファ（CBV）やライト用CBVの設定

    ///CBVを設定する

    //マテリアルCBufferの場所を設定(ここでの第一引数の0はRootParameter配列の0番目であり、registerの0ではない)
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, sprite->GetD3D12Resource()->materialResource_->GetGPUVirtualAddress());

    //wvp用のCbufferの場所を設定(今回はRootParameter[1]に対してCBVの設定を行っている)
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, sprite->GetD3D12Resource()->transformationResource_->GetGPUVirtualAddress());

    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, sprite->GetD3D12Resource()->directionalLightResource_->GetGPUVirtualAddress());

    // 5. テクスチャ用のDescriptor Table設定（SRV）

    /*テクスチャを貼ろう*/

    //SRVのDescriptorTableの先頭を設定。2はRootParameter[2]である。
    dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, sprite->GetD3D12Resource()->textureHandle_);

    // 6. 描画

    /*三角形を表示しよう*/

    //描画！(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
    dxCommon_->GetCommandList()->DrawIndexedInstanced(static_cast<UINT>(sprite->GetD3D12Resource()->indexDataList_.size()), 1, 0, 0, 0);

}

void DrawManager::DrawSphere(SphereClass* sphere) {

    /*三角形を表示しよう*/
    //RootSignatureを設定。PSOに設定しているけど別途指定が必要
    dxCommon_->GetCommandList()->SetGraphicsRootSignature(dxCommon_->GetRootSignature());
    dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &sphere->GetD3D12Resource()->vertexBufferView_); // VBVを設定
    //IBVを設定
    dxCommon_->GetCommandList()->IASetIndexBuffer(&sphere->GetD3D12Resource()->indexBufferView_);
    //形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
    dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    /*三角形の色を変えよう*/

    ///CBVを設定する

    //マテリアルCBufferの場所を設定(ここでの第一引数の0はRootParameter配列の0番目であり、registerの0ではない)
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, sphere->GetD3D12Resource()->materialResource_->GetGPUVirtualAddress());

    /*三角形を動かそう*/

    //wvp用のCbufferの場所を設定(今回はRootParameter[1]に対してCBVの設定を行っている)
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, sphere->GetD3D12Resource()->transformationResource_->GetGPUVirtualAddress());

    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, sphere->GetD3D12Resource()->directionalLightResource_->GetGPUVirtualAddress());

    /*PhongReflectionModel*/

    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(5, sphere->GetD3D12Resource()->cameraResource_->GetGPUVirtualAddress());

    /*テクスチャを貼ろう*/

    ///DescriptorTableを設定する

    //SRVのDescriptorTableの先頭を設定。2はRootParameter[2]である。
    dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, sphere->GetD3D12Resource()->textureHandle_);

    EnsurePointLightResource();
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(6, pointLight_->GetResource()->GetGPUVirtualAddress());

    EnsureSpotLightResource();
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(7, spotLight_->GetResource()->GetGPUVirtualAddress());

    /*三角形を表示しよう*/

    //描画！(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
    dxCommon_->GetCommandList()->DrawIndexedInstanced(static_cast<UINT>(sphere->GetD3D12Resource()->indexDataList_.size()), 1, 0, 0, 0);

}

void DrawManager::DrawParticle(ParticleClass* resource) {

    /*三角形を表示しよう*/
    //RootSignatureを設定。PSOに設定しているけど別途指定が必要
    dxCommon_->GetCommandList()->SetGraphicsRootSignature(dxCommon_->GetRootSignature());
    dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &resource->GetD3D12Resource()->vertexBufferView_); // VBVを設定
    //IBVを設定
    dxCommon_->GetCommandList()->IASetIndexBuffer(&resource->GetD3D12Resource()->indexBufferView_);
    //形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
    dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    /*三角形の色を変えよう*/

    ///CBVを設定する

    //マテリアルCBufferの場所を設定(ここでの第一引数の0はRootParameter配列の0番目であり、registerの0ではない)
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, resource->GetD3D12Resource()->materialResource_->GetGPUVirtualAddress());

    auto instancing = resource->GetInstancingSrvHandleGPU();
    assert(instancing.ptr != 0 && "Instancing SRV handle is null or invalid");
    dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(4, resource->GetInstancingSrvHandleGPU());

    /*テクスチャを貼ろう*/

    ///DescriptorTableを設定する

    //SRVのDescriptorTableの先頭を設定。2はRootParameter[2]である。
    dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, resource->GetD3D12Resource()->textureHandle_);

    /*三角形を表示しよう*/

    //描画！(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
    dxCommon_->GetCommandList()->DrawIndexedInstanced(static_cast<UINT>(resource->GetD3D12Resource()->indexDataList_.size()), resource->GetInstanceCount(), 0, 0, 0);

}

void DrawManager::DrawByIndex(D3D12ResourceUtil* resource) {

    /*三角形を表示しよう*/
    //RootSignatureを設定。PSOに設定しているけど別途指定が必要
    dxCommon_->GetCommandList()->SetGraphicsRootSignature(dxCommon_->GetRootSignature());
    dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &resource->vertexBufferView_); // VBVを設定
    //IBVを設定
    dxCommon_->GetCommandList()->IASetIndexBuffer(&resource->indexBufferView_);
    //形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
    dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    /*三角形の色を変えよう*/

    ///CBVを設定する

    //マテリアルCBufferの場所を設定(ここでの第一引数の0はRootParameter配列の0番目であり、registerの0ではない)
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, resource->materialResource_->GetGPUVirtualAddress());

    /*三角形を動かそう*/

    //wvp用のCbufferの場所を設定(今回はRootParameter[1]に対してCBVの設定を行っている)
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, resource->transformationResource_->GetGPUVirtualAddress());

    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, resource->directionalLightResource_->GetGPUVirtualAddress());

    /*PhongReflectionModel*/

    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(5, resource->cameraResource_->GetGPUVirtualAddress());

    /*テクスチャを貼ろう*/

    ///DescriptorTableを設定する

    //SRVのDescriptorTableの先頭を設定。2はRootParameter[2]である。
    dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, resource->textureHandle_);

    EnsurePointLightResource();
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(6, pointLight_->GetResource()->GetGPUVirtualAddress());

    EnsureSpotLightResource();
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(7, spotLight_->GetResource()->GetGPUVirtualAddress());

    /*三角形を表示しよう*/

    //描画！(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
    dxCommon_->GetCommandList()->DrawIndexedInstanced(static_cast<UINT>(resource->indexDataList_.size()), 1, 0, 0, 0);

}

void DrawManager::DrawByVertex(D3D12ResourceUtil* resource) {

    /*三角形を表示しよう*/
    //RootSignatureを設定。PSOに設定しているけど別途指定が必要
    dxCommon_->GetCommandList()->SetGraphicsRootSignature(dxCommon_->GetRootSignature());
    dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &resource->vertexBufferView_); // VBVを設定
    //形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
    dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    /*三角形の色を変えよう*/

    ///CBVを設定する

    //マテリアルCBufferの場所を設定(ここでの第一引数の0はRootParameter配列の0番目であり、registerの0ではない)
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, resource->materialResource_->GetGPUVirtualAddress());

    /*三角形を動かそう*/

    //wvp用のCbufferの場所を設定(今回はRootParameter[1]に対してCBVの設定を行っている)
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, resource->transformationResource_->GetGPUVirtualAddress());

    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, resource->directionalLightResource_->GetGPUVirtualAddress());

    /*PhongReflectionModel*/

    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(5, resource->cameraResource_->GetGPUVirtualAddress());

    /*テクスチャを貼ろう*/

    ///DescriptorTableを設定する

    //SRVのDescriptorTableの先頭を設定。2はRootParameter[2]である。
    dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, resource->textureHandle_);

    EnsurePointLightResource();
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(6, pointLight_->GetResource()->GetGPUVirtualAddress());

    EnsureSpotLightResource();
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(7, spotLight_->GetResource()->GetGPUVirtualAddress());

    /*三角形を表示しよう*/

    //描画！(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
    dxCommon_->GetCommandList()->DrawInstanced(static_cast<UINT>(resource->vertexDataList_.size()), 1, 0, 0);

}

