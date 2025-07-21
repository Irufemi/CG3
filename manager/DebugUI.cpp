#include "DebugUI.h"

#include <Windows.h>


/*開発のUIを出そう*/

#include "../externals/imgui/imgui.h"
#include "../externals/imgui/imgui_impl_dx12.h"
#include "../externals/imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void DebugUI::Initialize(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList, const Microsoft::WRL::ComPtr<ID3D12Device>& device, HWND& hwnd, DXGI_SWAP_CHAIN_DESC1& swapChainDesc, D3D12_RENDER_TARGET_VIEW_DESC& rtvDesc, ID3D12DescriptorHeap* srvDescriptorHeap) {
    
    this->commandList_ = commandList;

    /*開発UIを出そう*/
    //ImGuiの初期化。詳細はさして重要ではないので開設は省略する。
    //こういうもんである
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX12_Init(
        device.Get(),
        swapChainDesc.BufferCount,
        rtvDesc.Format,
        srvDescriptorHeap,
        srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
        srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
    );

}

void DebugUI::FrameStart() {

    /*開発のUIを出そう*/

    ///ImGuiを使う
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

}

void DebugUI::Shutdown() {

    if (commandList_) { commandList_.Reset(); }
    /*開発のUIを出そう*/

    ///ImGuiの終了処理

    //ImGuiの終了処理。詳細はさして重要ではないので解説は省略する。
    //こういうもんである。初期化と逆順に行う。
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

}

void DebugUI::QueueDrawCommands() {

    /*開発のUIを出そう*/

    ///ImGuiを使う

    //ImGuiの内部コマンドを生成する
    ImGui::Render();
}

void DebugUI::QueuePostDrawCommands() {

    /*開発のUIを出そう*/

    ///ImGuiを描画する

    //実際のcommandListのImGuiの描画コマンドを積む
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList_.Get());

}
