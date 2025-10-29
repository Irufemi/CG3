// Pixel shader for instanced sprites
// - テクスチャは PS:t0 にバインド（DrawManager のバインドと一致させること）
// - サンプラーは s0（ルートシグネチャに静的サンプラがあればそれを使う）

cbuffer MaterialCB : register(b0)
{
    float4 materialColor;
    int    enableLighting;
    int    hasTexture;
    int    lightingMode;
    int    pad0;
    float4x4 uvTransform;
};

Texture2D    gTexture : register(t0, space0);
SamplerState gSampler : register(s0);

struct PSInput {
    float4 color : COLOR0;
    float2 uv    : TEXCOORD0;
};

float4 PSMain(PSInput pin) : SV_Target {
    float4 base = (hasTexture != 0) ? gTexture.Sample(gSampler, pin.uv) : float4(1.0f, 1.0f, 1.0f, 1.0f);
    return base * pin.color;
}