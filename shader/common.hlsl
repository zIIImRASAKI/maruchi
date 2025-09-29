cbuffer WorldBuffer : register(b0)
{
	matrix World;
}
cbuffer ViewBuffer : register(b1)
{
	matrix View;
}
cbuffer ProjectionBuffer : register(b2)
{
	matrix Projection;
}

struct MATERIAL
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float4 Emission;
	float Shininess;
	bool TextureEnable;
	float2 Dummy;
};

cbuffer MaterialBuffer : register(b3)
{
	MATERIAL Material;
}

struct LIGHT
{
	bool Enable;					// 使用するか否か
	bool3 Dummy;					// PADDING
	float4 Direction;				// 方向
	float4 Diffuse;					// 拡散反射用の光の強さ
	float4 Ambient;					// 環境光用の光の強さ
};

cbuffer LightBuffer : register(b4)
{
    LIGHT Light;
};

#define MAX_BONE 400
cbuffer BoneMatrixBuffer : register(b5)
{
    matrix BoneMatrix[MAX_BONE];
}

cbuffer LINEWIDTH : register(b6)
{
    float4 LineWidth;
};
struct VS_IN
{
	float4 Position		: POSITION0;
	float4 Normal		: NORMAL0;
	float4 Diffuse		: COLOR0;
	float2 TexCoord		: TEXCOORD0;
};

struct VSONESKIN_IN
{
    float4 Position		: POSITION0;
    float4 Normal		: NORMAL0;
    float4 Diffuse		: COLOR0;
    float2 TexCoord		: TEXCOORD0;
    int4   BoneIndex	: BONEINDEX;
    float4 BoneWeight	: BONEWEIGHT;
};

struct PS_IN
{
	float4 Position		: SV_POSITION;
	float4 Diffuse		: COLOR0;
	float2 TexCoord		: TEXCOORD0;
};

// 3x3 逆行列を計算する関数
/*  警告はでたため修正
float3x3 Inverse3x3(float3x3 m)
{
    float a = m[0].x, b = m[0].y, c = m[0].z;
    float d = m[1].x, e = m[1].y, f = m[1].z;
    float g = m[2].x, h = m[2].y, i = m[2].z;

    float A = e * i - f * h;
    float B = f * g - d * i;
    float C = d * h - e * g;

    float det = a * A + b * B + c * C;
    if (abs(det) < 1e-6)
    {
        float3x3 zeroMatrix = float3x3(0.0f, 0.0f, 0.0f,
                              0.0f, 0.0f, 0.0f,
                              0.0f, 0.0f, 0.0f);
        return zeroMatrix; // 非正則ならゼロ行列       
    }

    float invDet = 1.0 / det;

    return float3x3(
         A, c * h - b * i, b * f - c * e,
         B, a * i - c * g, c * d - a * f,
         C, b * g - a * h, a * e - b * d
    ) * invDet;
}
*/
/*
/*  警告はでたため修正
float3x3 Inverse3x3(float3x3 m)
{
    // 全要素を単位行列で初期化（警告対策）
    float3x3 invMat = float3x3(1, 0, 0, 0, 1, 0, 0, 0, 1);
    
    float a00 = m[0][0], a01 = m[0][1], a02 = m[0][2];
    float a10 = m[1][0], a11 = m[1][1], a12 = m[1][2];
    float a20 = m[2][0], a21 = m[2][1], a22 = m[2][2];

    float b01 = a22 * a11 - a12 * a21;
    float b11 = -a22 * a10 + a12 * a20;
    float b21 = a21 * a10 - a11 * a20;

    float det = a00 * b01 + a01 * b11 + a02 * b21;

    // ゼロ割防止用（det が小さい場合は単位行列を返す）
    if (abs(det) < 1e-8)
    {
        return transpose(invMat);
    }

    invMat[0][0] = b01 / det;
    invMat[0][1] = (-a22 * a01 + a02 * a21) / det;
    invMat[0][2] = (a12 * a01 - a02 * a11) / det;

    invMat[1][0] = b11 / det;
    invMat[1][1] = (a22 * a00 - a02 * a20) / det;
    invMat[1][2] = (-a12 * a00 + a02 * a10) / det;

    invMat[2][0] = b21 / det;
    invMat[2][1] = (-a21 * a00 + a01 * a20) / det;
    invMat[2][2] = (a11 * a00 - a01 * a10) / det;

    return transpose(invMat);
}
*/
static const float EPS = 1e-8;

float3x3 Inverse3x3(float3x3 m) // returns transpose(inverse(m))
{
    // 余因子の一部
    float a00 = m[0][0], a01 = m[0][1], a02 = m[0][2];
    float a10 = m[1][0], a11 = m[1][1], a12 = m[1][2];
    float a20 = m[2][0], a21 = m[2][1], a22 = m[2][2];

    float c00 = a11 * a22 - a12 * a21;  // b01
    float c01 = -a10 * a22 + a12 * a20; // b11
    float c02 = a10 * a21 - a11 * a20;  // b21

    float det = a00 * c00 + a01 * c01 + a02 * c02;

    // det が小さいときは 0、十分大きいときは 1
    float s = step(EPS, abs(det));          // 0 or 1
    float signNZ = 2.0 * step(0.0, det) - 1.0; // det<0 → -1、det>=0 → +1（0でも+1）
    float invDet = rcp(max(abs(det), EPS)) * signNZ; // 1 / det を符号付きで安全に


    // 逆行列の転置（adj(A)^T / det）を直接組み立て
    float3x3 N;
    N[0][0] = c00 * invDet;
    N[0][1] = (-a01 * a22 + a02 * a21) * invDet;
    N[0][2] = (a01 * a12 - a02 * a11) * invDet;

    N[1][0] = c01 * invDet;
    N[1][1] = (a00 * a22 - a02 * a20) * invDet;
    N[1][2] = (-a00 * a12 + a02 * a10) * invDet;

    N[2][0] = c02 * invDet;
    N[2][1] = (-a00 * a21 + a01 * a20) * invDet;
    N[2][2] = (a00 * a11 - a01 * a10) * invDet;

    // 特異時は単位行列、正常時は N を返す（分岐なしで常に初期化済み）
    float3x3 I = float3x3(1, 0, 0, 0, 1, 0, 0, 0, 1);
    return I * (1.0 - s) + N * s;
}

