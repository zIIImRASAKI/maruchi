#pragma once
#include <array>
#include <cmath>
#include <algorithm>
#include "../system/commontypes.h"
#include "../system/CPlane.h"
#include "../system/renderer.h"

/**
 * @brief 任意平面上に幅×高さの長方形を構築し、TRIANGLESTRIP用4頂点を生成するクラス
 *
 * - 平面は n·x + d = 0（nは単位長を想定）
 * - 接ベクトル (u, v) は自動算出。Axisヒント指定／回転で向き調整が可能
 * - 退化（法線ゼロ・接ベクトルが作れない）を考慮し bool で返す
 */
class PlaneQuad {
public:

    /** @brief 既定コンストラクタ */
    PlaneQuad() = default;

    /**
     * @brief 構築用コンストラクタ
     * @param planeInfo 平面情報（CPlane::MakeEquation の出力推奨）
     * @param center    長方形中心（平面上）
     * @param width     幅
     * @param height    高さ
     */
    PlaneQuad(const PLANEINFO& planeInfo, const Vector3& center, float width, float height)
        : m_planeinfo(planeInfo), m_center(center), m_width(width), m_height(height) {
    }

    // --- 設定系 ---
    void   SetPlane(const PLANEINFO& pi) { m_planeinfo = pi; }
    void   SetPlane(const Plane& p) { m_planeinfo.plane = p; }            ///< p0,p1,p2は任意
    void   SetCenter(const Vector3& c) { m_center = c; }
    void   SetSize(float w, float h) { m_width = w; m_height = h; }
    void   SetAxisHint(const Vector3& a) { m_axisHint = a; m_hasAxisHint = true; }
    void   ClearAxisHint() { m_hasAxisHint = false; }
    void   SetUVRect(const Vector2& uvMin, const Vector2& uvMax) { m_uvMin = uvMin; m_uvMax = uvMax; }

    /**
     * @brief u,v を法線周りに回転（ラジアン）。Build前に呼ぶと向きを微調整できる
     * @param angleRad +で反時計回り（u→v方向）
     */
    void   SetRotateAroundNormal(float angleRad) { m_rotateRad = angleRad; }

    /** @brief 中心を「原点の平面射影点」に設定（center未定の簡便用） */
    void   AutoCenterFromPlane() {
        Vector3 n(m_planeinfo.plane.a, m_planeinfo.plane.b, m_planeinfo.plane.c);
        float nlen2 = n.LengthSquared();
        if (nlen2 > 1e-12f) {
            n /= std::sqrt(nlen2);
            m_center = -m_planeinfo.plane.d * n;
        }
    }

    // --- 生成 ---
    /**
     * @brief TRIANGLESTRIP用4頂点を生成（LT, RT, LB, RB の順）
     * @param outVerts 生成結果
     * @return 成功ならtrue（退化時はfalse）
     *
     * 並びは (v0,v1,v2,v3) = (LT, RT, LB, RB)。
     * 三角形は (v0,v1,v2) と (v1,v3,v2)。カリングONだと片面が消える点に注意。
     */
    bool BuildStrip(std::array<VERTEX_3D, 4>& outVerts) const;

    // --- 取得 ---
    const PLANEINFO& GetPlaneInfo() const noexcept { return m_planeinfo; }
    Vector3          GetCenter() const noexcept { return m_center; }
    float            GetWidth()  const noexcept { return m_width; }
    float            GetHeight() const noexcept { return m_height; }

private:
    static constexpr float kEps = 1e-6f;

    // 平面・ジオメトリ設定
    PLANEINFO m_planeinfo{};
    Vector3   m_center{ 0,0,0 };
    float     m_width = 1.0f;
    float     m_height = 1.0f;

    // UV設定
    Vector2   m_uvMin{ 0.0f, 0.0f };
    Vector2   m_uvMax{ 1.0f, 1.0f };

    // 向き調整
    bool      m_hasAxisHint = false;
    Vector3   m_axisHint{ 1,0,0 };  ///< 法線と独立なヒント。u ≈ normalize( (axisHint × n) ) を利用
    float     m_rotateRad = 0.0f;   ///< u,v を法線周りに回転

    // 補助：接ベクトルを作る
    bool MakeTangentFrame(Vector3& n, Vector3& u, Vector3& v) const;
};
