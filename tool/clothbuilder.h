#pragma once
#include <vector>
#include <utility>
#include <cstddef>
#include <cassert>
#include "verletobject.h"
#include "link.h"

// 既存の定義を流用する想定（なければこの2つも本ヘッダで定義してください）
struct ClothHandle {
    std::vector<size_t> bodyIds;                          // 作った質点のID
    std::vector<std::pair<size_t, size_t>> links;         // バネの両端インデックス
};

struct ClothSpringParams {
    float k;   // ばね定数
    float cn;  // 法線ダンピング係数
    float ct;  // 接線ダンピング係数
};

/**
 * @brief 格子状の布（Hookeばねネット）を構築するビルダー
 *
 * - rows × cols の質点を spacing 間隔で格子状に配置
 * - 構造（4近傍）・せん断（斜め）・曲げ（2マス離れ）の各ばねを生成
 * - 必要に応じて上辺の左右端を固定（pin）
 * - グローバル擬似速度減衰 gamma を各質点に設定
 *
 * 使い方:
 * @code
 * ClothGridBuilder::Config cfg{
 *     .rows = 20, .cols = 30, .spacing = 0.2f,
 *     .origin = Vector3(0, 1, 0), .massPerNode = 0.05f,
 *     .structParams = { 0.8f, 0.05f, 0.02f },
 *     .shearParams  = { 0.6f, 0.03f, 0.02f },
 *     .bendParams   = { 0.2f, 0.02f, 0.01f },
 *     .gamma = 0.5f, .pinTopCorners = true
 * };
 *
 * ClothGridBuilder builder{cfg};
 * ClothHandle cloth = builder.build(world);
 * @endcode
 */
class ClothGridBuilder {
public:
    struct Config {
        int     rows = 0;                           // 行数
        int     cols = 0;                           // 列数
        float   spacing = 0.0f;                     // 間隔
        float   radius = 5.0f;
        Vector3 origin = Vector3(0, 0, 0);          // 左上原点
        float   massPerNode = 1.0f;                 // 質量
        ClothSpringParams structParams{ 0,0,0 };    // 構造バネ係数パラメータ
        ClothSpringParams shearParams{ 0,0,0 };     // せん断バネ係数パラメータ
        ClothSpringParams bendParams{ 0,0,0 };      // 曲げバネ係数パラメータ
        float   gamma = 0.0f;                       // グローバル擬似速度減衰（1/s, 0で無効）
        bool    pinTopCorners = false;              // 左上と右上を固定化するか否か
    };

    explicit ClothGridBuilder(const Config& cfg) noexcept
        : m_config(cfg) {
    }

    /** @brief 設定を取得 */
    const Config& config() const noexcept { return m_config; }

    /** @brief 設定を更新（ビルダーを再利用したい場合に） */
    void setConfig(const Config& cfg) noexcept { m_config = cfg; }

    /**
     * @brief 布を構築して 指定のVerletObjectコンテナに登録します
     */
    ClothHandle build(
        std::vector<std::unique_ptr<VerletObject>>& volist,
        std::vector<std::unique_ptr<Link>>& linklist);

private:
    Config m_config;

    // 1次元配列のインデックスを計算
    size_t idx(int r, int c) const noexcept {
        return 
            static_cast<size_t>(r) * static_cast<size_t>(m_config.cols)
            + static_cast<size_t>(c);
    }

    void addSpringIfValid(
        std::vector<std::unique_ptr<VerletObject>>& volist,
        std::vector<std::unique_ptr<Link>>& linklist,
        ClothHandle& h,
        int r1, int c1, int r2, int c2,
        const ClothSpringParams& p);
};
