#include	"player.h"	
#include    "../system/CDirectInput.h"

void player::init() {

	m_mesh = std::make_unique<CStaticMesh>();
	m_mesh->Load("assets/model/car000.x", "assets/model");

	m_meshrenderer = std::make_unique<CStaticMeshRenderer>();
	m_meshrenderer->Init(*m_mesh);

	m_shader = std::make_unique<CShader>();
	m_shader->Create("shader/vertexLightingVS.hlsl","shader/vertexLightingPS.hlsl");

	m_srt.pos = Vector3(0, 0, 0);
	m_srt.scale = Vector3(1, 1, 1);
	m_srt.rot = Vector3(0, 0, 0);
}

void player::update(uint64_t dt) {

	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_A)) {
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_W))
		{// 左前移動

			float radian;
			radian = PI * 0.75f;

			m_move.x -= sinf(radian) * VALUE_MOVE_MODEL;
			m_move.z -= cosf(radian) * VALUE_MOVE_MODEL;

			// 目標角度をセット
			m_destrot.y = radian;

		}
		else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_S))
		{// 左後移動

			float radian;
			radian = PI * 0.25f;

			m_move.x -= sinf(radian) * VALUE_MOVE_MODEL;
			m_move.z -= cosf(radian) * VALUE_MOVE_MODEL;

			// 目標角度をセット
			m_destrot.y = radian;
		}
		else
		{// 左移動

			float radian;
			radian = PI * 0.50f;

			m_move.x -= sinf(radian) * VALUE_MOVE_MODEL;
			m_move.z -= cosf(radian) * VALUE_MOVE_MODEL;

			// 目標角度をセット
			m_destrot.y = radian;
		}
	}

	else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_D))
	{
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_W)) {
			// 右前移動

			float radian;
			radian = -PI * 0.75f;

			m_move.x -= sinf(radian) * VALUE_MOVE_MODEL;
			m_move.z -= cosf(radian) * VALUE_MOVE_MODEL;

			// 目標角度をセット
			m_destrot.y = radian;

		}
		else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_S))
		{// 右後移動
			float radian;
			radian = -PI * 0.25f;

			m_move.x -= sinf(radian) * VALUE_MOVE_MODEL;
			m_move.z -= cosf(radian) * VALUE_MOVE_MODEL;

			// 目標角度をセット
			m_destrot.y = radian;
		}
		else
		{// 右移動

			float radian;
			radian = -PI * 0.50f;

			m_move.x -= sinf(radian) * VALUE_MOVE_MODEL;
			m_move.z -= cosf(radian) * VALUE_MOVE_MODEL;

			// 目標角度をセット
			m_destrot.y = radian;
		}
	}
	else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_W))
	{// 前移動
		float radian;
		radian = PI;

		m_move.x -= sinf(radian) * VALUE_MOVE_MODEL;
		m_move.z -= cosf(radian) * VALUE_MOVE_MODEL;

		// 目標角度をセット
		m_destrot.y = PI;
	}
	else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_S))
	{// 後移動
		float radian;
		radian = 0.0f;

		m_move.x -= sinf(radian) * VALUE_MOVE_MODEL;
		m_move.z -= cosf(radian) * VALUE_MOVE_MODEL;

		// 目標角度をセット
		m_destrot.y = 0.0f;
	}

	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_RIGHT))
	{// 左回転
		m_destrot.y = m_srt.rot.y - VALUE_ROTATE_MODEL;
		if (m_destrot.y < -PI)
		{
			m_destrot.y += PI * 2.0f;
		}
	}

	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_LEFT))
	{// 右回転
		m_destrot.y = m_srt.rot.y + VALUE_ROTATE_MODEL;
		if (m_destrot.y > PI)
		{
			m_destrot.y -= PI * 2.0f;
		}
	}

	// 目標角度と現在角度との差分を求める
	float diffrot = m_destrot.y - m_srt.rot.y;
	if (diffrot > PI)
	{
		diffrot -= PI * 2.0f;
	}
	if (diffrot < -PI)
	{
		diffrot += PI * 2.0f;
	}

	// 比率計算
	m_srt.rot.y += diffrot * RATE_ROTATE_MODEL;
	if (m_srt.rot.y > PI)
	{
		m_srt.rot.y -= PI * 2.0f;
	}
	if (m_srt.rot.y < -PI)
	{
		m_srt.rot.y += PI * 2.0f;
	}


	/// 位置移動
	m_srt.pos += m_move;

	// 移動量に慣性をかける(減速率)
	m_move += -m_move * RATE_MOVE_MODEL;

	// リセット
	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_RETURN))
	{// リセット
		m_srt.pos = Vector3(0.0f, 0.0f, 0.0f);
		m_srt.rot = Vector3(0.0f, 0.0f, 0.0f);
	}

}

void player::draw(uint64_t dt) {


	Matrix4x4 mtx = m_srt.GetMatrix();

	Renderer::SetWorldMatrix(&mtx);

	m_shader->SetGPU();
	m_meshrenderer->Draw();

}

void player::dispose() {

}
