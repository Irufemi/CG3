#include "Math.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

namespace Math {



#pragma region 3次元ベクトル関数

    //加算
    Vector3 Add(const Vector3& a, const Vector3& b) {

        return { a.x + b.x,a.y + b.y,a.z + b.z };

    }

    //減算
    Vector3 Subtract(const Vector3& a, const Vector3& b) {

        return { a.x - b.x,a.y - b.y,a.z - b.z };
    }

    //スカラー倍
    Vector3 Multiply(const float scalar, const Vector3 vector) {

        return { vector.x * scalar,vector.y * scalar,vector.z * scalar };
    }

    //内積(a ・ b)
    float Dot(const Vector3& a, const Vector3& b) {

        return { a.x * b.x + a.y * b.y + a.z * b.z };
    }

    //ノルム(長さ)( ||v|| )
    float Length(const Vector3& vector) {

        return std::sqrt(Dot(vector, vector));

    }

    //正規化
    Vector3 Normalize(const Vector3& vector) {

        return Multiply(1.0f / Length(vector), vector);

    }

    //クロス積（外積）
    Vector3 Cross(const Vector3& a, const Vector3& b) {

        return  { a.y * b.z - a.z * b.y,a.z * b.x - a.x * b.z,a.x * b.y - a.y * b.x };

    }

#pragma endregion


#pragma region 4x4行列関数

    // 4x4行列の加法
    Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2) {
        Matrix4x4 addResult{};

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                addResult.m[i][j] = m1.m[i][j] + m2.m[i][j];
            }
        }

        return addResult;
    }

    // 4x4行列の減法
    Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2) {
        Matrix4x4 subtractResult{};

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                subtractResult.m[i][j] = m1.m[i][j] - m2.m[i][j];
            }
        }

        return subtractResult;
    }

    // 4x4行列の積
    Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
        Matrix4x4 multiplyResult{};
        multiplyResult.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0];
        multiplyResult.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1];
        multiplyResult.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2];
        multiplyResult.m[0][3] = m1.m[1][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3];
        multiplyResult.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0];
        multiplyResult.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1];
        multiplyResult.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2];
        multiplyResult.m[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3];
        multiplyResult.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0];
        multiplyResult.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1];
        multiplyResult.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2];
        multiplyResult.m[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3];
        multiplyResult.m[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0];
        multiplyResult.m[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1];
        multiplyResult.m[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2];
        multiplyResult.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3];
        return multiplyResult;
    }

    // 4x4逆行列を求める 
    Matrix4x4 Inverse(const Matrix4x4& m) {
        Matrix4x4 inv{};

        const float* a = &m.m[0][0];
        float* o = &inv.m[0][0];

        o[0] = a[5] * a[10] * a[15] - a[5] * a[11] * a[14] - a[9] * a[6] * a[15] + a[9] * a[7] * a[14] + a[13] * a[6] * a[11] - a[13] * a[7] * a[10];
        o[1] = -a[1] * a[10] * a[15] + a[1] * a[11] * a[14] + a[9] * a[2] * a[15] - a[9] * a[3] * a[14] - a[13] * a[2] * a[11] + a[13] * a[3] * a[10];
        o[2] = a[1] * a[6] * a[15] - a[1] * a[7] * a[14] - a[5] * a[2] * a[15] + a[5] * a[3] * a[14] + a[13] * a[2] * a[7] - a[13] * a[3] * a[6];
        o[3] = -a[1] * a[6] * a[11] + a[1] * a[7] * a[10] + a[5] * a[2] * a[11] - a[5] * a[3] * a[10] - a[9] * a[2] * a[7] + a[9] * a[3] * a[6];
        o[4] = -a[4] * a[10] * a[15] + a[4] * a[11] * a[14] + a[8] * a[6] * a[15] - a[8] * a[7] * a[14] - a[12] * a[6] * a[11] + a[12] * a[7] * a[10];
        o[5] = a[0] * a[10] * a[15] - a[0] * a[11] * a[14] - a[8] * a[2] * a[15] + a[8] * a[3] * a[14] + a[12] * a[2] * a[11] - a[12] * a[3] * a[10];
        o[6] = -a[0] * a[6] * a[15] + a[0] * a[7] * a[14] + a[4] * a[2] * a[15] - a[4] * a[3] * a[14] - a[12] * a[2] * a[7] + a[12] * a[3] * a[6];
        o[7] = a[0] * a[6] * a[11] - a[0] * a[7] * a[10] - a[4] * a[2] * a[11] + a[4] * a[3] * a[10] + a[8] * a[2] * a[7] - a[8] * a[3] * a[6];
        o[8] = a[4] * a[9] * a[15] - a[4] * a[11] * a[13] - a[8] * a[5] * a[15] + a[8] * a[7] * a[13] + a[12] * a[5] * a[11] - a[12] * a[7] * a[9];
        o[9] = -a[0] * a[9] * a[15] + a[0] * a[11] * a[13] + a[8] * a[1] * a[15] - a[8] * a[3] * a[13] - a[12] * a[1] * a[11] + a[12] * a[3] * a[9];
        o[10] = a[0] * a[5] * a[15] - a[0] * a[7] * a[13] - a[4] * a[1] * a[15] + a[4] * a[3] * a[13] + a[12] * a[1] * a[7] - a[12] * a[3] * a[5];
        o[11] = -a[0] * a[5] * a[11] + a[0] * a[7] * a[9] + a[4] * a[1] * a[11] - a[4] * a[3] * a[9] - a[8] * a[1] * a[7] + a[8] * a[3] * a[5];
        o[12] = -a[4] * a[9] * a[14] + a[4] * a[10] * a[13] + a[8] * a[5] * a[14] - a[8] * a[6] * a[13] - a[12] * a[5] * a[10] + a[12] * a[6] * a[9];
        o[13] = a[0] * a[9] * a[14] - a[0] * a[10] * a[13] - a[8] * a[1] * a[14] + a[8] * a[2] * a[13] + a[12] * a[1] * a[10] - a[12] * a[2] * a[9];
        o[14] = -a[0] * a[5] * a[14] + a[0] * a[6] * a[13] + a[4] * a[1] * a[14] - a[4] * a[2] * a[13] - a[12] * a[1] * a[6] + a[12] * a[2] * a[5];
        o[15] = a[0] * a[5] * a[10] - a[0] * a[6] * a[9] - a[4] * a[1] * a[10] + a[4] * a[2] * a[9] + a[8] * a[1] * a[6] - a[8] * a[2] * a[5];

        float det = a[0] * o[0] + a[1] * o[4] + a[2] * o[8] + a[3] * o[12];

        if (det == 0.0f) {
            return Matrix4x4(); // 逆行列が存在しない場合（ゼロ行列返すなど）
        }

        float invDet = 1.0f / det;
        for (int i = 0; i < 16; ++i) {
            o[i] *= invDet;
        }

        return inv;
    }

    // 4x4転置行列を求める 
    Matrix4x4 Transpose(const Matrix4x4& m) {
        Matrix4x4 tMatrix{};
        tMatrix.m[0][0] = m.m[0][0];
        tMatrix.m[0][1] = m.m[1][0];
        tMatrix.m[0][2] = m.m[2][0];
        tMatrix.m[0][3] = m.m[3][0];
        tMatrix.m[1][0] = m.m[0][1];
        tMatrix.m[1][1] = m.m[1][1];
        tMatrix.m[1][2] = m.m[2][1];
        tMatrix.m[1][3] = m.m[3][1];
        tMatrix.m[2][0] = m.m[0][2];
        tMatrix.m[2][1] = m.m[1][2];
        tMatrix.m[2][2] = m.m[2][2];
        tMatrix.m[2][3] = m.m[3][2];
        tMatrix.m[3][0] = m.m[0][3];
        tMatrix.m[3][1] = m.m[1][3];
        tMatrix.m[3][2] = m.m[2][3];
        tMatrix.m[3][3] = m.m[2][3];
        return tMatrix;
    }

    //4x4単位行列の作成
    Matrix4x4 MakeIdentity4x4() {
        Matrix4x4 m = {
            1.0f,0.0f,0.0f,0.0f,
            0.0f,1.0f,0.0f,0.0f,
            0.0f,0.0f,1.0f,0.0f,
            0.0f,0.0f,0.0f,1.0f
        };
        return m;
    }

    // 4x4平行移動行列の作成関数 
    Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
        Matrix4x4 resultTranslateMatrix{};
        resultTranslateMatrix.m[0][0] = 1.0f;
        resultTranslateMatrix.m[0][1] = 0.0f;
        resultTranslateMatrix.m[0][2] = 0.0f;
        resultTranslateMatrix.m[0][3] = 0.0f;
        resultTranslateMatrix.m[1][0] = 0.0f;
        resultTranslateMatrix.m[1][1] = 1.0f;
        resultTranslateMatrix.m[1][2] = 0.0f;
        resultTranslateMatrix.m[1][3] = 0.0f;
        resultTranslateMatrix.m[2][0] = 0.0f;
        resultTranslateMatrix.m[2][1] = 0.0f;
        resultTranslateMatrix.m[2][2] = 1.0f;
        resultTranslateMatrix.m[2][3] = 0.0f;
        resultTranslateMatrix.m[3][0] = translate.x;
        resultTranslateMatrix.m[3][1] = translate.y;
        resultTranslateMatrix.m[3][2] = translate.z;
        resultTranslateMatrix.m[3][3] = 1.0f;
        return resultTranslateMatrix;
    }

    //4x4拡大縮小行列の作成関数
    Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
        Matrix4x4 resultScaleMtrix{};
        resultScaleMtrix.m[0][0] = scale.x;
        resultScaleMtrix.m[0][1] = 0.0f;
        resultScaleMtrix.m[0][2] = 0.0f;
        resultScaleMtrix.m[0][3] = 0.0f;
        resultScaleMtrix.m[1][0] = 0.0f;
        resultScaleMtrix.m[1][1] = scale.y;
        resultScaleMtrix.m[1][2] = 0.0f;
        resultScaleMtrix.m[1][3] = 0.0f;
        resultScaleMtrix.m[2][0] = 0.0f;
        resultScaleMtrix.m[2][1] = 0.0f;
        resultScaleMtrix.m[2][2] = scale.z;
        resultScaleMtrix.m[2][3] = 0.0f;
        resultScaleMtrix.m[3][0] = 0.0f;
        resultScaleMtrix.m[3][1] = 0.0f;
        resultScaleMtrix.m[3][2] = 0.0f;
        resultScaleMtrix.m[3][3] = 1.0f;
        return resultScaleMtrix;
    }

    // 3次元ベクトルを同次座標として変換する 
    Vector3 Transform(const Vector3& vector, const Matrix4x4& m) {
        Vector3 transformResult{};
        transformResult.x = vector.x * m.m[0][0] + vector.y * m.m[1][0] + vector.z * m.m[2][0] + 1.0f * m.m[3][0];
        transformResult.y = vector.x * m.m[0][1] + vector.y * m.m[1][1] + vector.z * m.m[2][1] + 1.0f * m.m[3][1];
        transformResult.z = vector.x * m.m[0][2] + vector.y * m.m[1][2] + vector.z * m.m[2][2] + 1.0f * m.m[3][2];
        float w = vector.x * m.m[0][3] + vector.y * m.m[1][3] + vector.z * m.m[2][3] + 1.0f * m.m[3][3];
        if (w != 0.0f) { ///ベクトルに対して基本的な操作を行う行列でwが0になることはありえない
            transformResult.x /= w; //w=1がデカルト座標系であるので、w除算することで同時座標をデカルト座標に戻す
            transformResult.y /= w;
            transformResult.z /= w;
            return transformResult;
        }
        else {
            return { 0 };
        }
    }

    // 4x4 X軸周り回転行列の作成関数
    Matrix4x4 MakeRotateXMatrix(const float& radian) {
        Matrix4x4 matrix{};
        matrix.m[0][0] = 1.0f;
        matrix.m[0][1] = 0.0f;
        matrix.m[0][2] = 0.0f;
        matrix.m[0][3] = 0.0f;
        matrix.m[1][0] = 0.0f;
        matrix.m[1][1] = std::cos(radian);
        matrix.m[1][2] = std::sin(radian);
        matrix.m[1][3] = 0.0f;
        matrix.m[2][0] = 0.0f;
        matrix.m[2][1] = -std::sin(radian);
        matrix.m[2][2] = std::cos(radian);
        matrix.m[2][3] = 0.0f;
        matrix.m[3][0] = 0.0f;
        matrix.m[3][1] = 0.0f;
        matrix.m[3][2] = 0.0f;
        matrix.m[3][3] = 1.0f;
        return matrix;
    }

    // 4x4 Y軸周り回転行列の作成関数
    Matrix4x4 MakeRotateYMatrix(const float& radian) {
        Matrix4x4 matrix{};
        matrix.m[0][0] = std::cos(radian);
        matrix.m[0][1] = 0.0f;
        matrix.m[0][2] = -std::sin(radian);
        matrix.m[0][3] = 0.0f;
        matrix.m[1][0] = 0.0f;
        matrix.m[1][1] = 1.0f;
        matrix.m[1][2] = 0.0f;
        matrix.m[1][3] = 0.0f;
        matrix.m[2][0] = std::sin(radian);
        matrix.m[2][1] = 0.0f;
        matrix.m[2][2] = std::cos(radian);
        matrix.m[2][3] = 0.0f;
        matrix.m[3][0] = 0.0f;
        matrix.m[3][1] = 0.0f;
        matrix.m[3][2] = 0.0f;
        matrix.m[3][3] = 1.0f;
        return matrix;
    }

    // 4x4 Z軸周り回転行列の作成関数
    Matrix4x4 MakeRotateZMatrix(const float& radian) {
        Matrix4x4 matrix{};
        matrix.m[0][0] = std::cos(radian);
        matrix.m[0][1] = std::sin(radian);
        matrix.m[0][2] = 0.0f;
        matrix.m[0][3] = 0.0f;
        matrix.m[1][0] = -std::sin(radian);
        matrix.m[1][1] = std::cos(radian);
        matrix.m[1][2] = 0.0f;
        matrix.m[1][3] = 0.0f;
        matrix.m[2][0] = 0.0f;
        matrix.m[2][1] = 0.0f;
        matrix.m[2][2] = 1.0f;
        matrix.m[2][3] = 0.0f;
        matrix.m[3][0] = 0.0f;
        matrix.m[3][1] = 0.0f;
        matrix.m[3][2] = 0.0f;
        matrix.m[3][3] = 1.0f;
        return matrix;
    }

    // 3次元回転行列の作成関数
    Matrix4x4 MakeRotateXYZMatrix(const float& thetaX, const float& thetaY, const float& thetaZ) {
        Matrix4x4 matrix = Multiply(MakeRotateXMatrix(thetaX), Multiply(MakeRotateYMatrix(thetaY), MakeRotateZMatrix(thetaZ)));
        return matrix;
    }

    //4x4アフィン変換行列を高速に生成
    Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
        Matrix4x4 rotateMatrix = MakeRotateXYZMatrix(rotate.x, rotate.y, rotate.z);
        Matrix4x4 affineMatrix{};
        affineMatrix.m[0][0] = scale.x * rotateMatrix.m[0][0];
        affineMatrix.m[0][1] = scale.x * rotateMatrix.m[0][1];
        affineMatrix.m[0][2] = scale.x * rotateMatrix.m[0][2];
        affineMatrix.m[0][3] = 0.0f;
        affineMatrix.m[1][0] = scale.y * rotateMatrix.m[1][0];
        affineMatrix.m[1][1] = scale.y * rotateMatrix.m[1][1];
        affineMatrix.m[1][2] = scale.y * rotateMatrix.m[1][2];
        affineMatrix.m[1][3] = 0.0f;
        affineMatrix.m[2][0] = scale.z * rotateMatrix.m[2][0];
        affineMatrix.m[2][1] = scale.z * rotateMatrix.m[2][1];
        affineMatrix.m[2][2] = scale.z * rotateMatrix.m[2][2];
        affineMatrix.m[2][3] = 0.0f;
        affineMatrix.m[3][0] = translate.x;
        affineMatrix.m[3][1] = translate.y;
        affineMatrix.m[3][2] = translate.z;
        affineMatrix.m[3][3] = 1.0f;
        return affineMatrix;
    }

    //透視投影行列の作成
    Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
        Matrix4x4 perspectiveFovMatrix{};
        perspectiveFovMatrix.m[0][0] = 1.0f / aspectRatio * (1.0f / std::tan(fovY / 2.0f));
        perspectiveFovMatrix.m[0][1] = 0.0f;
        perspectiveFovMatrix.m[0][2] = 0.0f;
        perspectiveFovMatrix.m[0][3] = 0.0f;
        perspectiveFovMatrix.m[1][0] = 0.0f;
        perspectiveFovMatrix.m[1][1] = 1.0f / std::tan(fovY / 2.0f);
        perspectiveFovMatrix.m[1][2] = 0.0f;
        perspectiveFovMatrix.m[1][3] = 0.0f;
        perspectiveFovMatrix.m[2][0] = 0.0f;
        perspectiveFovMatrix.m[2][1] = 0.0f;
        perspectiveFovMatrix.m[2][2] = farClip / (farClip - nearClip);
        perspectiveFovMatrix.m[2][3] = 1.0f;
        perspectiveFovMatrix.m[3][0] = 0.0f;
        perspectiveFovMatrix.m[3][1] = 0.0f;
        perspectiveFovMatrix.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);
        perspectiveFovMatrix.m[3][3] = 0.0f;
        return perspectiveFovMatrix;
    }

    //正射行列の作成
    Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
        Matrix4x4 projectionMatrix{};
        projectionMatrix.m[0][0] = 2.0f / (right - left);
        projectionMatrix.m[0][1] = 0.0f;
        projectionMatrix.m[0][2] = 0.0f;
        projectionMatrix.m[0][3] = 0.0f;
        projectionMatrix.m[1][0] = 0.0f;
        projectionMatrix.m[1][1] = 2.0f / (top - bottom);
        projectionMatrix.m[1][2] = 0.0f;
        projectionMatrix.m[1][3] = 0.0f;
        projectionMatrix.m[2][0] = 0.0f;
        projectionMatrix.m[2][1] = 0.0f;
        projectionMatrix.m[2][2] = 1.0f / (farClip - nearClip);
        projectionMatrix.m[2][3] = 0.0f;
        projectionMatrix.m[3][0] = (left + right) / (left - right);
        projectionMatrix.m[3][1] = (top + bottom) / (bottom - top);
        projectionMatrix.m[3][2] = nearClip / (nearClip - farClip);
        projectionMatrix.m[3][3] = 1.0f;
        return projectionMatrix;
    }

    //ビューポート変換行列の作成
    Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
        Matrix4x4 viewportMatrix{};
        viewportMatrix.m[0][0] = width / 2.0f;
        viewportMatrix.m[0][1] = 0.0f;
        viewportMatrix.m[0][2] = 0.0f;
        viewportMatrix.m[0][3] = 0.0f;
        viewportMatrix.m[1][0] = 0.0f;
        viewportMatrix.m[1][1] = -height / 2.0f;
        viewportMatrix.m[1][2] = 0.0f;
        viewportMatrix.m[1][3] = 0.0f;
        viewportMatrix.m[2][0] = 0.0f;
        viewportMatrix.m[2][1] = 0.0f;
        viewportMatrix.m[2][2] = maxDepth - minDepth;
        viewportMatrix.m[2][3] = 0.0f;
        viewportMatrix.m[3][0] = left + width / 2.0f;
        viewportMatrix.m[3][1] = top + height / 2.0f;
        viewportMatrix.m[3][2] = minDepth;
        viewportMatrix.m[3][3] = 1.0f;
        return viewportMatrix;
    }

#pragma endregion
}