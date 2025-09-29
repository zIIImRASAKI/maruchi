#include "common.hlsl"

PS_IN main(in VSONESKIN_IN In)
{
    PS_IN Out;

	// ワンスキン頂点ブレンドの処理
    float4x4 comb = (float4x4) 0;
    for (int i = 0; i < 4; i++)
    {
		// 重みを計算しながら行列生成
        comb += BoneMatrix[In.BoneIndex[i]] * In.BoneWeight[i];
    }

    float4 Pos;

    Pos = mul(In.Position,comb);
    In.Position = Pos;

    // 法線ベクトルの補正
    float3x3 normalcombMatrix = Inverse3x3(float3x3(comb._11, comb._12, comb._13,
                                             comb._21, comb._22, comb._23,
                                             comb._31, comb._32, comb._33));    // 逆行列を求める
    normalcombMatrix = transpose(normalcombMatrix); // 転置

    float3 n;
    n = In.Normal.xyz;
    
    float3 combNormal = mul(n, normalcombMatrix);
    combNormal = normalize(combNormal);

    In.Normal = float4(combNormal, 0.0f);
     
	// World View Projection行列を合成
    matrix wvp;
    wvp = mul(World, View);
    wvp = mul(wvp, Projection);
	
    // 法線変換行列を計算（拡縮成分を取り除く）
    float3x3 normalMatrix = Inverse3x3(float3x3(World._11, World._12, World._13,
                                             World._21, World._22, World._23,
                                             World._31, World._32, World._33));
    // 転置
    normalMatrix = transpose(normalMatrix);

    // 法線ベクトルの方向をワールド座標系に変換
    float3 worldNormal, normal;
    normal = In.Normal.xyz;
    
    worldNormal = mul(normal, normalMatrix);
    worldNormal = normalize(worldNormal);

    normal = In.Normal.xyz;
    worldNormal = mul(normal, (float3x3)World);
    worldNormal = normalize(worldNormal);

    float light = -(dot(Light.Direction.xyz, worldNormal.xyz))* 0.5 + 0.5;
    light = saturate(light);  // ランバート反射の計算

    Out.Diffuse = In.Diffuse * Material.Diffuse * light * Light.Diffuse;
    Out.Diffuse += In.Diffuse * Material.Ambient * Light.Ambient;
    Out.Diffuse += Material.Emission;
    Out.Diffuse.a = In.Diffuse.a * Material.Diffuse.a;

    Out.Position = mul(In.Position, wvp);
    Out.TexCoord = In.TexCoord;

    return Out;    
}
