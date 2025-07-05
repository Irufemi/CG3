#pragma once

#include <cstdint>
#define DIRECTINPUT_VERSION 0x0800 //DirectInputのバージョン指定
#include <dinput.h>
#include <array>

class InputManager {

    IDirectInput8* directInput = nullptr;

    IDirectInputDevice8* keybord = nullptr;

    std::array<BYTE, 256> key_ = {};

    std::array<BYTE, 256> preKey_ = {};

public:
    //コンストラクタ
    InputManager(){}
    //デストラクタ
    ~InputManager(){
        directInput->Release();
        keybord->Release();
    }
    //初期化
    void Initialize(WNDCLASS &wc, HWND &hwnd);
    //更新
    void Update();

    //
    void GetHitKeyStateAll(char* keyStateSelf);
    //
    const std::array<BYTE, 256>& GetAllKey() const { return key_; }

    ///トリガー処理

    //キーを押した状態か
    bool isKeyDown(uint8_t key);
    //キーを離した状態か
    bool isKeyUp(uint8_t key);
    //キーを押した瞬間か
    bool isKeyPress(uint8_t key);
    //キーを離した瞬間か
    bool isKeyRelease(uint8_t key);
};
