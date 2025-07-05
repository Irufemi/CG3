#pragma once

#include "Vector3.h"
#include "Matrix4x4.h"

namespace Math {

#pragma region 3次元ベクトル関数

    /// <summary>
    /// 加算
    /// </summary>
    /// <param name="a"></param>
    /// <param name="b"></param>
    /// <returns></returns>
    Vector3 Add(const Vector3& a, const Vector3& b);

    /// <summary>
    /// 減算
    /// </summary>
    /// <param name="a"></param>
    /// <param name="b"></param>
    /// <returns></returns>
    Vector3 Subtract(const Vector3& a, const Vector3& b);

    /// <summary>
    /// スカラー倍
    /// </summary>
    /// <param name="scalar"></param>
    /// <param name="vector"></param>
    /// <returns></returns>
    Vector3 Multiply(const float scalar, const Vector3 vector);

    /// <summary>
    /// 内積
    /// </summary>
    /// <param name="a"></param>
    /// <param name="b"></param>
    /// <returns></returns>
    float Dot(const Vector3& a, const Vector3& b);


    /// <summary>
    /// ノルム(長さ)
    /// </summary>
    /// <param name="vector"></param>
    /// <returns></returns>
    float Length(const Vector3& vector);

    /// <summary>
    /// 正規化
    /// </summary>
    /// <param name="vector"></param>
    /// <returns></returns>
    Vector3 Normalize(const Vector3& vector);

    /// <summary>
    /// クロス積（外積）
    /// </summary>
    /// <param name="a"></param>
    /// <param name="b"></param>
    /// <returns></returns>
    Vector3 Cross(const Vector3& a, const Vector3& b);

#pragma endregion

#pragma region 4x4行列関数

    /// <summary>
    /// 4x4行列の加法
    /// </summary>
    /// <param name="m1"></param>
    /// <param name="m2"></param>
    /// <returns></returns>
    Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);

    /// <summary>
    /// 4x4行列の減法
    /// </summary>
    /// <param name="m1"></param>
    /// <param name="m2"></param>
    /// <returns></returns>
    Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2);

    /// <summary>
    /// 4x4行列の積
    /// </summary>
    /// <param name="vector"></param>
    /// <param name="m"></param>
    /// <returns></returns>
    Matrix4x4 Multiply(const Matrix4x4& vector, const Matrix4x4& m);

    /// <summary>
    /// 4x4逆行列を求める
    /// </summary>
    /// <param name="m"></param>
    /// <returns></returns>
    Matrix4x4 Inverse(const Matrix4x4& m);

    /// <summary>
    /// 4x4転置行列を求める 
    /// </summary>
    /// <param name="m"></param>
    /// <returns></returns>
    Matrix4x4 Transpose(const Matrix4x4& m);

    /// <summary>
    /// 4x4単位行列の作成
    /// </summary>
    /// <returns></returns>
    Matrix4x4 MakeIdentity4x4();

    /// <summary>
    /// 4x4平行移動行列の作成
    /// </summary>
    /// <param name="translate"></param>
    /// <returns></returns>
    Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

    /// <summary>
    /// 4x4拡大縮小行列の作成
    /// </summary>
    /// <param name="scale"></param>
    /// <returns></returns>
    Matrix4x4 MakeScaleMatrix(const Vector3& scale);

    /// <summary>
    /// 3次元ベクトルを同次座標として変換
    /// </summary>
    /// <param name="vector"></param>
    /// <param name="matrix"></param>
    /// <returns></returns>
    Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);


    //回転は左手座標系(左手を握って親指の方向が軸、その他の指の幕方向が回転の正の方向)

    /// <summary>
    /// 4x4 X軸周り回転行列の作成
    /// </summary>
    /// <param name="theta"></param>
    /// <returns></returns>
    Matrix4x4 MakeRotateXMatrix(const float& theta);

    /// <summary>
    /// 4x4 Y軸周り回転行列の作成
    /// </summary>
    /// <param name="theta"></param>
    /// <returns></returns>
    Matrix4x4 MakeRotateYMatrix(const float& theta);

    /// <summary>
    /// 4x4 Z軸周り回転行列の作成
    /// </summary>
    /// <param name="theta"></param>
    /// <returns></returns>
    Matrix4x4 MakeRotateZMatrix(const float& theta);

    /// <summary>
    /// 4x4 3次元回転行列の作成
    /// </summary>
    /// <param name="thetaX"></param>
    /// <param name="thetaY"></param>
    /// <param name="thetaZ"></param>
    /// <returns></returns>
    Matrix4x4 MakeRotateXYZMatrix(const float& thetaX, const float& thetaY, const float& thetaZ);

    /// <summary>
    /// 4x4アフィン変換行列を高速に生成
    /// </summary>
    /// <param name="scale"></param>
    /// <param name="rotate"></param>
    /// <param name="translate"></param>
    /// <returns></returns>
    Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

    /// <summary>
    /// 透視投影行列の作成
    /// </summary>
    /// <param name="fovY"></param>
    /// <param name="aspectRatio"></param>
    /// <param name="nearClip"></param>
    /// <param name="farClip"></param>
    /// <returns></returns>
    Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

    /// <summary>
    /// 正射行列の作成
    /// </summary>
    /// <param name="left"></param>
    /// <param name="top"></param>
    /// <param name="right"></param>
    /// <param name="bottom"></param>
    /// <param name="nearClip"></param>
    /// <param name="farClip"></param>
    /// <returns></returns>
    Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

    /// <summary>
    /// ビューポート変換行列の作成
    /// </summary>
    /// <param name="left"></param>
    /// <param name="top"></param>
    /// <param name="width"></param>
    /// <param name="height"></param>
    /// <param name="minDepth"></param>
    /// <param name="maxDepth"></param>
    /// <returns></returns>
    Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

#pragma endregion


}
