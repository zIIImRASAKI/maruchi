#pragma once
#include "commontypes.h"

/**
 * @brief 平面 (n·x + d = 0) の係数
 * @note a,b,c に法線ベクトル n、d にオフセット。n は正規化済みを保証する実装にしている。
 */
struct Plane {
    float a{}, b{}, c{}, d{};
};

/**
 * @brief 地形面の平面方程式と、その生成に使った3頂点
 */
struct PLANEINFO {
    Plane   plane;      ///< 平面方程式 n·x + d = 0（nは単位長）
    Vector3 p0, p1, p2; ///< 生成に使った頂点
};

/**
 * @brief 平面ユーティリティ
 *
 * - 3点からの生成（退化チェック付き）
 * - 点＋法線からの生成
 * - 符号付き距離、点の射影、法線反転 など
 */
class CPlane {
public:
    /// @brief 3点から平面を生成（右手系・頂点順序で法線が決まる）
    /// @param p0,p1,p2 平面上の3点（非共線）
    /// @param forceUpNormal true のとき、法線y<0なら反転して “上向き” に揃える
    /// @return 成功なら true（退化三角形は false）
    bool MakeEquation(const Vector3& p0, const Vector3& p1, const Vector3& p2, bool forceUpNormal = false);

    /// @brief 点と法線から平面を生成（法線は自動正規化）
    /// @param point 平面上の任意点
    /// @param normal 法線ベクトル（ゼロ長不可）
    /// @return 成功なら true
    bool MakeFromPointNormal(const Vector3& point, const Vector3& normal);

    /// @brief 平面情報を返す
    const PLANEINFO& GetPlaneInfo() const noexcept { return m_planeinfo; }

    /// @brief 単位法線ベクトルを取得
    Vector3 Normal() const noexcept { return { m_planeinfo.plane.a, m_planeinfo.plane.b, m_planeinfo.plane.c }; }

    /// @brief 符号付き距離 s = n·x + d（n は単位長）
    float SignedDistance(const Vector3& x) const noexcept { return Normal().Dot(x) + m_planeinfo.plane.d; }

    /// @brief 点 x を平面に正射影
    Vector3 ProjectPoint(const Vector3& x) const noexcept { return x - Normal() * SignedDistance(x); }

    /// @brief 法線と d を反転（n→-n, d→-d）
    void Flip() noexcept;

private:
    static constexpr float kEps = 1e-6f;
    PLANEINFO m_planeinfo{};
};
