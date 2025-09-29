#include <cmath>
#include <algorithm>
#include "commontypes.h"
#include "CPlane.h"

/**
 * @brief 3点から平面を生成
 * @details 法線 n = (p1-p0) × (p2-p0) を正規化し、d = -n·p0 とする。
 *          3点がほぼ共線（退化）なら失敗。
 */
bool CPlane::MakeEquation(const Vector3& p0, const Vector3& p1, const Vector3& p2, bool forceUpNormal)
{
    // 2本の独立な辺ベクトル（同一点起点を推奨）
    const Vector3 u = p1 - p0;
    const Vector3 v = p2 - p0;

    // 外積で法線
    Vector3 n = u.Cross(v);
    const float len2 = n.LengthSquared();
    if (len2 < kEps) {
        // 退化（三点が一直線上 or 同一点に近い）
        return false;
    }
    n /= std::sqrt(len2); // 正規化

    // 任意に“上向き”に揃えたいとき
    if (forceUpNormal && n.y < 0.0f) n = -n;

    Plane p;
    p.a = n.x; p.b = n.y; p.c = n.z;
    p.d = -n.Dot(p0); // n·x + d = 0

    m_planeinfo.plane = p;
    m_planeinfo.p0 = p0;
    m_planeinfo.p1 = p1;
    m_planeinfo.p2 = p2;
    return true;
}

/**
 * @brief 点と法線から平面を生成（法線は自動正規化）
 */
bool CPlane::MakeFromPointNormal(const Vector3& point, const Vector3& normal)
{
    const float len2 = normal.LengthSquared();
    if (len2 < kEps) return false;

    Vector3 n = normal / std::sqrt(len2);

    Plane p;
    p.a = n.x; p.b = n.y; p.c = n.z;
    p.d = -n.Dot(point);

    m_planeinfo.plane = p;
    m_planeinfo.p0 = point;
    m_planeinfo.p1 = point; // 参照用に point を入れておく（未使用）
    m_planeinfo.p2 = point;
    return true;
}

/**
 * @brief 法線と d を反転（n→-n, d→-d）
 */
void CPlane::Flip() noexcept
{
    m_planeinfo.plane.a = -m_planeinfo.plane.a;
    m_planeinfo.plane.b = -m_planeinfo.plane.b;
    m_planeinfo.plane.c = -m_planeinfo.plane.c;
    m_planeinfo.plane.d = -m_planeinfo.plane.d;
}
