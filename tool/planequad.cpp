#include "../system/commontypes.h"
#include "planequad.h"

/** @brief 接ベクトル (u,v) を生成。nは正規化して返す */
bool PlaneQuad::MakeTangentFrame(Vector3& n, Vector3& u, Vector3& v) const
{
    // 法線を正規化
    n = Vector3(m_planeinfo.plane.a, m_planeinfo.plane.b, m_planeinfo.plane.c);
    float nlen2 = n.LengthSquared();
    if (nlen2 < 1e-12f) return false;
    n /= std::sqrt(nlen2);

    // u の初期候補
    bool ok = false;

    if (m_hasAxisHint) {
        // ヒントから生成：u0 = axisHint × n
        Vector3 u0 = m_axisHint.Cross(n);
        float ul2 = u0.LengthSquared();
        if (ul2 > 1e-12f) {
            u = u0 / std::sqrt(ul2);
            ok = true;
        }
    }

    // ヒントが無い／使えない場合：三角形の辺 or 代替軸から生成
    if (!ok) {
        Vector3 u0 = m_planeinfo.p1 - m_planeinfo.p0;
        if (u0.LengthSquared() < 1e-12f) {
            u0 = m_planeinfo.p2 - m_planeinfo.p0;
        }
        if (u0.LengthSquared() < 1e-12f) {
            // n と独立な軸を選び、u = n × a とする
            Vector3 a = (std::fabs(n.x) < 0.9f) ? Vector3(1, 0, 0) : Vector3(0, 1, 0);
            u0 = n.Cross(a);
        }
        // 平面内へ正射影してから正規化（数値安定）
        u0 = u0 - n * u0.Dot(n);
        float ul2 = u0.LengthSquared();
        if (ul2 < 1e-12f) return false;
        u = u0 / std::sqrt(ul2);
    }

    // v = n × u
    v = n.Cross(u);
    float vl2 = v.LengthSquared();
    if (vl2 < 1e-12f) return false;
    v /= std::sqrt(vl2);

    // 任意回転（u,v を法線周りに）
    if (m_rotateRad != 0.0f) {
        const float c = std::cos(m_rotateRad);
        const float s = std::sin(m_rotateRad);
        Vector3 u2 = u * c + v * s;
        Vector3 v2 = -u * s + v * c;
        u = u2; v = v2;
    }
    return true;
}

bool PlaneQuad::BuildStrip(std::array<VERTEX_3D, 4>& outVerts) const
{
    // 幅高さチェック
    if (!(std::isfinite(m_width) && std::isfinite(m_height))) return false;

    // 接ベクトル作成
    Vector3 n, u, v;
    if (!MakeTangentFrame(n, u, v)) return false;

    const float hw = m_width * 0.5f;
    const float hh = m_height * 0.5f;

    // 四隅（LeftTop, RightTop, LeftBottom, RightBottom）
    const Vector3 LT = m_center + (-hw) * u + (+hh) * v;
    const Vector3 RT = m_center + (+hw) * u + (+hh) * v;
    const Vector3 LB = m_center + (-hw) * u + (-hh) * v;
    const Vector3 RB = m_center + (+hw) * u + (-hh) * v;

    // UV矩形
    const Vector2 uvLT = { m_uvMin.x, m_uvMin.y };
    const Vector2 uvRT = { m_uvMax.x, m_uvMin.y };
    const Vector2 uvLB = { m_uvMin.x, m_uvMax.y };
    const Vector2 uvRB = { m_uvMax.x, m_uvMax.y };

    // TRIANGLESTRIP順：LT, RT, LB, RB
    outVerts[0] = { LT, n, Color(1,1,1,1),uvLT };
    outVerts[1] = { RT, n, Color(1,1,1,1),uvRT };
    outVerts[2] = { LB, n, Color(1,1,1,1),uvLB };
    outVerts[3] = { RB, n, Color(1,1,1,1),uvRB };
    return true;
}
