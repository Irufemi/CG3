
/*テクスチャを貼ろう*/

#include "./Object3d.hlsli"

/*三角形の色を変えよう*/

struct Material
{
	float32_t4 color;
	
	/*LambertianReflectance*/
	
	int32_t enableLighting;
	
	int32_t hasTexture;
	
	 // 0=Lightingなし, 1=Lambert, 2=HalfLambert
	int32_t lightingMode;
	
	float padding;
	
	/*UVTransform*/
	
	///Materialの拡張
	
	float32_t4x4 uvTransform;

	float32_t shininess;
	
};
ConstantBuffer<Material> gMaterial : register(b0);
struct PixelShaderOutput
{
	float32_t4 color : SV_TARGET0;
};

/*テクスチャを貼ろう*/

///Textureを使う

Texture2D<float32_t4> gTexture : register(t0); //SRVのregisterはt
SamplerState gSampler : register(s0); //Samplerのregisterはs

/*LambertianReflectance*/

struct DirectionalLight
{
	 //!< ライトの色
	float32_t4 color;
    //!< ライトの向き
	float32_t3 direction;
    //!< 輝度
	float intensity;
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

/*PhongReflectionModel*/

/// カメラの位置を送る

struct Camera
{
	float32_t3 worldPosition;
};
ConstantBuffer<Camera> gCamera : register(b2);

/*テクスチャを貼ろう*/

PixelShaderOutput main(VertexShaderOutput input)
{
	PixelShaderOutput output;
	
	/*UVTransform*/
	
	///Materialを拡張する
	
	float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
	
	/*2値抜き*/
		
	/// disxard
		
	// textureのα値が0.5以下の時にPixelを棄却
	if (textureColor.a <= 0.5)
	{
		discard;
	}
		
	// textureのα値が0の時にPixelを棄却
	if (textureColor.a == 0.0)
	{
		discard;
	}
	
	/*テクスチャを貼ろう*/
	
	///Lightingの計算を行う
	
	if (gMaterial.enableLighting != 0) //Lightingする場合
	{
	
		float cos = 1.0f;
	
		if (gMaterial.lightingMode == 0)
		{
			if (gMaterial.hasTexture == 1)
			{
				output.color = gMaterial.color * textureColor;
			}
			else
			{
				output.color = gMaterial.color;
				output.color.a = 1.0f;
			}
		}
		else
		{
			if (gMaterial.lightingMode == 1)
			{
				cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
				output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
			}
			else if (gMaterial.lightingMode == 2)
			{
		
				/*HalfLambert*/
		
				///HalfLambertを実装する
		
				//HalfLambert
				float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
				cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
		
			}

			/*PhongReflectionModel*/

			/// カメラの位置を送る
	
			// Cameraへの方向を算出
			float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);

			/// 反射ベクトルと強度を求める
	
			// 入射光の反射ベクトルを求める
			float32_t3 reflectRight = reflect(gDirectionalLight.direction, normalize(input.normal));
	
			//// 内積をとる
			//float RdotE = dot(reflectRight, toEye);
			//// 鏡面反射の強度を求める
			//float specularPow = pow(saturate(RdotE), gMaterial.shininess);
			
			/*BlinnPhongReflectionModel*/
			
			/// HalfVectorを求めて計算する
			
			float32_t3 halfVector = normalize(-gDirectionalLight.direction + toEye);
			float NDotH = dot(normalize(input.normal), halfVector);
			float specularPow = pow(saturate(NDotH), gMaterial.shininess);

			/*PhongReflectionModel*/
	
			/// すべてを1つに
			// 拡散反射
			float32_t3 diffuse = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
			// 鏡面反射
			float32_t3 specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
			// 拡散反射 + 鏡面反射
			output.color.rgb = diffuse + specular;
			
			if (gMaterial.hasTexture == 1)
			{
				// アルファは今まで通り
				output.color.a = gMaterial.color.a * textureColor.a;
			}
			else
			{
				output.color.a = 1.0f;
			}
		}
		
		/*2値抜き*/
		
		/// disxard
		
		// output.aolorのα値が0の時にPixelを棄却
		if (output.color.a == 0.0)
		{
			discard;
		}
	}
	else
	{
	
		if (gMaterial.hasTexture == 1)
		{
			output.color = gMaterial.color * textureColor;
		}
		else
		{
			output.color = gMaterial.color;
			output.color.a = 1.0f;
		}
		
	}
	
	return output;
}


