#include "InputManager.h"

#include <cassert>

void InputManager::Initialize(WNDCLASS& wc, HWND& hwnd) {

    /*入力デバイス*/

    ///初期化(一度だけ行う処理)

    //DirectInputの初期化
    HRESULT hr = DirectInput8Create(wc.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
    assert(SUCCEEDED(hr));

    //キーボードデバイスの生成
    hr = directInput->CreateDevice(GUID_SysKeyboard, &keybord, NULL);
    assert(SUCCEEDED(hr));

    //入力データ形式のセット
    hr = keybord->SetDataFormat(&c_dfDIKeyboard); //標準形式
    assert(SUCCEEDED(hr));

    //排他制御レベルのセット
    hr = keybord->SetCooperativeLevel(
        hwnd,
        DISCL_FOREGROUND //画面が手前にある場合のみ入力を受け付ける
        | DISCL_NONEXCLUSIVE //デバイスをこのアプリだけで専有しない
        | DISCL_NOWINKEY //Windowsキーを無効にする
    );
    assert(SUCCEEDED(hr));

}

void InputManager::Update() {

    //preKeyにkeyの情報をコピー
    memcpy(preKey_.data(), key_.data(), 256);

    /*入力デバイス*/

    ///グラフィックスコマンド

    ///更新処理(毎フレーム行う)

    //キーボード情報の取得開始
    keybord->Acquire();

    //全キーの入力状態を取得する
    keybord->GetDeviceState(sizeof(key_), key_.data());

}

void InputManager::GetHitKeyStateAll(char* keyStateBuf) {
    memcpy(keyStateBuf, key_.data(), size(key_));
}

///トリガー処理

//キーを押した状態か
bool InputManager::isKeyDown(uint8_t keyCode) {
    if (key_[keyCode] != 0) {
        return true;
    }
    return false;
}

//キーを離した状態か
bool InputManager::isKeyUp(uint8_t keyCode) {
    if (key_[keyCode] == 0) {
        return true;
    }
    return false;
}

//キーを押した瞬間か
bool InputManager::isKeyPress(uint8_t keyCode) {
    if (preKey_[keyCode] == 0 && key_[keyCode] != 0) {
        return true;
    }
    return false;
}

//キーを離した瞬間か
bool InputManager::isKeyRelease(uint8_t keyCode) {
    if (preKey_[keyCode] != 0 && key_[keyCode] == 0) {
        return true;
    }
    return false;
}
