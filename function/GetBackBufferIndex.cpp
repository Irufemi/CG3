#include "GetBackBufferIndex.h"

#include <cassert>

UINT GetBackBufferIndex(IDXGISwapChain4* swapChain) {
    assert(swapChain != nullptr);
    return swapChain->GetCurrentBackBufferIndex();
}