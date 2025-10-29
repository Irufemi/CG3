// Vertex shader for instanced sprites
// - Instance SRV expected in VS at t0 (例: register(t0, space1) をルートシグネチャに合わせて調整)
// - Material CBV expected at b0 (同様にルートシグネチャと一致させること)

struct InstanceData {
    float4x4 WVP;   // CPU側で計算済み WVP
    float4   color; // インスタンス色
};

cbuffer MaterialCB : register(b0)
{
    float4 materialColor;
    int    enableLighting;
    int    hasTexture;
    int    lightingMode;
    int    pad0; // アライン用
    float4x4 uvTransform;
};

// インスタンス用 SRV (VS)
StructuredBuffer<InstanceData> gInstances : register(t0, space1);

// 頂点入力 (Sprite 側の頂点レイアウトに合わせる)
// POSITION : float4, TEXCOORD0 : float2, NORMAL : float3
struct VSInput {
    float4 position : POSITION;
    float2 uv       : TEXCOORD0;
    float3 normal   : NORMAL;
};

struct VSOutput {
    float4 svpos : SV_POSITION;
    float2 uv    : TEXCOORD0;
    float4 color : COLOR0;
};

VSOutput VSMain(VSInput vin, uint instId : SV_InstanceID) {
    VSOutput vout;

    // 位置変換（CPUで作ったWVPをそのまま使用）
    vout.svpos = mul(vin.position, gInstances[instId].WVP);

    // UV変換（行ベクトル x 行列 を想定）
    float3 uvIn = float3(vin.uv, 1.0f);
    float3 uvOut = mul(uvIn, (float3x3)uvTransform);
    vout.uv = uvOut.xy;

    // インスタンス色 × マテリアル色
    vout.color = gInstances[instId].color * materialColor;

    return vout;
}