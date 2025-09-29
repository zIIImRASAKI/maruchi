#pragma once

#include    "commontypes.h"
#include	"renderer.h"
#include	"NonCopyable.h"

struct BoneCombMatrix : NonCopyable 
{
	CBBoneCombMatrix	 ConstantBufferMemory{};					// 行列群
	ComPtr<ID3D11Buffer> ConstantBuffer{};			// 定数バッファ

	BoneCombMatrix() {}
	~BoneCombMatrix() 
	{
	}

	// 定数バッファ生成し内容を更新する
	bool Create();

	// 定数バッファを更新する
	void Update();

	// GPUにセット
	void SetGPU();

	// 初期化
	void InitConstantBufferMemory() {
		for (int cnt = 0; cnt < MAX_BONE; cnt++) {
			ConstantBufferMemory.BoneCombMtx[cnt] = Matrix4x4::Identity;
		}
	}
};