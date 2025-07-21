#pragma once

#include "Vector4.h"
#include "Matrix4x4.h"
#include <cstdint>

/*LambertianReflectance*/

/// マテリアルを拡張する

struct Material {

    Vector4 color;
    int32_t enableLighting = false;

    /*UVTransform*//*UVTransform*/
	
    ///Alignmentを考慮して書き換え

    float padding[3];

    ///Materialの拡張

    Matrix4x4 uvTransform;

};