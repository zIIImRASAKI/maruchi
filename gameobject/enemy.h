#pragma once

#include <memory>
#include <array>
#include "gameobject.h"
#include "../system/CStaticMesh.h"
#include "../system/CStaticMeshRenderer.h"
#include "../system/CShader.h"
#include "../system/IScene.h"
#include "TankDefinition.h"

class enemy : public gameobject {

public:
    // コンストラクタ
    enemy(IScene* currentscene)
        : m_ownerscene(currentscene) {}

    // 毎フレーム更新
    void update(uint64_t delta) override;

    // 毎フレーム描画
    void draw(uint64_t delta) override;

    // 初期化
    void init() override;

    // リソース解放
    void dispose() override;

private:
    // 戦車の各パーツ
    std::array<std::unique_ptr<CStaticMesh>, PARTSMAX> m_pmeshes;               // メッシュデータ
    std::array<std::unique_ptr<CStaticMeshRenderer>, PARTSMAX> m_pmeshrenderers; // レンダラー

    std::array<Matrix4x4, PARTSMAX> m_localpose; // ローカル変換行列
    std::array<Matrix4x4, PARTSMAX> m_globalpose; // グローバル変換行列
    std::array<Matrix4x4, PARTSMAX> m_diffmtx;    // 初期姿勢からの差分

    SRT m_srt;                 // 自身の位置・回転・スケール
    Vector3 m_destrot = { 0.0f,0.0f,0.0f }; // 目標回転
    Vector3 m_move = { 0.0f,0.0f,0.0f };    // 移動量

    float m_speed = 0.1f; // 移動速度

    IScene* m_ownerscene = nullptr; // 所属シーン

    // 定数
    const float RATE_ROTATE_MODEL = 0.40f; // 回転慣性係数
    const float RATE_MOVE_MODEL = 0.20f;   // 移動慣性係数

    // ローカルポーズ生成
    void generateLocalPoses();

    // 指定パーツのグローバルポーズ計算
    Matrix4x4 calculateGlobalPose(size_t partIndex);

    // 全パーツのグローバルポーズ生成
    void generateGlobalPoses();
};
