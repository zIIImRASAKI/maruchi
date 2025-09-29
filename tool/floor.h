#pragma once
#include <array>
#include <cmath>
#include <algorithm>
#include "planequad.h"                  // あなたの CPlaneQuad ヘッダ
#include "../system/commontypes.h"
#include "../system/CPlane.h"

/**
 * @brief 任意平面上に長方形の「床」を生成/編集するクラス
 *
 * - 幾何: CPlane（平面） + CPlaneQuad（平面上クワッド）
 * - 形状: center, width, height, normal（ImGuiで編集）
 * - 出力: TRIANGLESTRIP 用の4頂点（pos/normal/uv）
 */
class Floor {
public:
    using Vertex = VERTEX_3D;

    /// @brief 既定: 原点中心 / 幅高さ=10 / 法線=(0,1,0)
    Floor()
        : m_center(0, 0, 0), m_width(10.0f), m_height(10.0f), m_normal(0, 1, 0) 
    {
    }

    /// @brief 構築
    Floor(const Vector3& center, float width, float height, const Vector3& normal)
        : m_center(center), m_width(width), m_height(height), m_normal(normal) 
    {
        RefreshPlaneAndQuad();
    }

    // --- セッター ---
    void SetCenter(const Vector3& c) { m_center = c; RefreshPlaneAndQuad(); }
    void SetSize(float w, float h) { m_width = w; m_height = h; RefreshPlaneAndQuad(); }
    void SetNormal(const Vector3& n) { m_normal = n; RefreshPlaneAndQuad(); }
    void SetUV(const Vector2& uvMin, const Vector2& uvMax) { m_uvMin = uvMin; m_uvMax = uvMax; m_quad.SetUVRect(uvMin, uvMax); }
    void SetAxisHint(const Vector3& a) { m_axisHint = a; m_hasAxisHint = true;  m_quad.SetAxisHint(a); }
    void ClearAxisHint() { m_hasAxisHint = false;                 m_quad.ClearAxisHint(); }
    void SetRotateRad(float rad) { m_rotateRad = rad;                     m_quad.SetRotateAroundNormal(rad); }

    // --- ゲッター ---
    const Plane& GetPlane() const { return m_plane.GetPlaneInfo().plane; }
    const PLANEINFO& GetPlaneInfo() const { return m_plane.GetPlaneInfo(); }
    Vector3          GetCenter() const { return m_center; }
    float            GetWidth() const { return m_width; }
    float            GetHeight() const { return m_height; }
    Vector3          GetNormalUnit() const {
        Vector3 n(m_plane.GetPlaneInfo().plane.a, m_plane.GetPlaneInfo().plane.b, m_plane.GetPlaneInfo().plane.c);
        return n;
    }

    /// @brief 現在の床平面への符号付き距離 s = n·x + d
    float SignedDistance(const Vector3& x) const;
    /// @brief TRIANGLESTRIP用4頂点を生成（LT, RT, LB, RB）
    bool BuildStrip(std::array<Vertex, 4>& outVerts) const;

private:
    // パラメータ
    Vector3 m_center;
    float   m_width = 10.0f;
    float   m_height = 10.0f;
    Vector3 m_normal = Vector3(0, 1, 0); // 入力値（未正規化でも可）

    // UV と向き
    Vector2 m_uvMin{ 0,0 }, m_uvMax{ 1,1 };
    bool    m_hasAxisHint = false;
    Vector3 m_axisHint{ 1,0,0 };
    float   m_rotateRad = 0.0f;

    // 構成要素
    CPlane     m_plane;   ///< n·x + d = 0
    PlaneQuad  m_quad;    ///< 平面上クワッド

    /// @brief 内部更新（平面←center+normal、クワッド←plane/center/size）
    void RefreshPlaneAndQuad();
};
