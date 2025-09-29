#include    "floor.h"

/// @brief 現在の床平面への符号付き距離 s = n·x + d
float Floor::SignedDistance(const Vector3& x) const {
	const Plane& pl = m_plane.GetPlaneInfo().plane;
	return Vector3(pl.a, pl.b, pl.c).Dot(x) + pl.d;
}

/// @brief TRIANGLESTRIP用4頂点を生成（LT, RT, LB, RB）
bool Floor::BuildStrip(std::array<Vertex, 4>& outVerts) const {
	return m_quad.BuildStrip(outVerts);
}


/// @brief 内部更新（平面←center+normal、クワッド←plane/center/size）
void Floor::RefreshPlaneAndQuad() {
	// 法線を正規化して平面を更新
	Vector3 n = m_normal;
	if (n.LengthSquared() < 1e-12f) n = Vector3(0, 1, 0);
	n.Normalize();
	m_plane.MakeFromPointNormal(m_center, n);

	// CPlaneQuad に反映
	m_quad.SetPlane(m_plane.GetPlaneInfo());
	m_quad.SetCenter(m_center);
	m_quad.SetSize(m_width, m_height);
	m_quad.SetUVRect(m_uvMin, m_uvMax);
	m_quad.SetRotateAroundNormal(m_rotateRad);
	if (m_hasAxisHint) m_quad.SetAxisHint(m_axisHint);
	else               m_quad.ClearAxisHint();
}
