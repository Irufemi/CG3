#pragma once

#include "math/Vector3.h"
#include "math/Vector4.h"

struct PointLight {
    //!< ライトの色
    Vector4 color;
    //!< ライトの位置
    Vector3 position;
    //!< 輝度
    float intensity;
};