#include "../tool/udpcom.h"
#include "../tool/toml.hpp"

#include <string>
#include <array>
#include <iostream>
#include <random>


#include "../main.h"
#include "../system/DebugUI.h"
#include "../system/SphereDrawer.h"
#include "../system/LineDrawer.h"
#include "../system/CDirectInput.h"
#include "../system/meshmanager.h"

#include "../system/RandomEngine.h"

#include "CarDriveScene.h"
//#include "../tool/NetowrkobjectSearch.h"


namespace
{
}

void CarDriveScene::PositionInfoHandler(
	std::unique_ptr<MsgData> msg,
	uint32_t /*ipadr*/,
	uint16_t /*port*/)
{
	const int senderID = msg->Msg.Header.ID;     
	if (senderID < 0 || senderID > 2) return;
	if (senderID == m_myID) return;              
	
	int firstID = -1, secondID = -1;
	for (int id = 0; id < 3; ++id) {
		if (id == m_myID) continue;
		if (firstID == -1) firstID = id;
		else { secondID = id; break; }
	}
	const int mirrorIndex = (senderID == firstID) ? 0 : 1;

	if (mirrorIndex < 0 || mirrorIndex >= static_cast<int>(ENEMYMAX)) return;
	enemy* e = m_enemies[mirrorIndex].get();
	if (!e) return;

	SRT srt;
	srt.pos = msg->Msg.posinfobody.pos;
	srt.rot = msg->Msg.posinfobody.rotation;
	srt.scale = msg->Msg.posinfobody.scale;

	e->setSRT(srt);
}

// 平行光源の方向セット（保留）
void CarDriveScene::debugDirectionalLight()
{
	static Vector4 direction = Vector4(0.0f, 0.0f, 1.0f, 0.0f); // Z軸+方向に光を当てる	

	ImGui::Begin("debug Directional Light");

	ImGui::SliderFloat3("direction ", &direction.x, -1, 1);
	direction.Normalize();										// 正規化

	LIGHT light{};
	light.Enable = true;
	light.Direction = direction;

	light.Direction.Normalize();
	light.Ambient = Color(0.2f, 0.2f, 0.2f, 1.0f);
	light.Diffuse = Color(1.0f, 1.0f, 1.0f, 1.0f);

	Vector4 Direction = Vector4(direction.x, direction.y, direction.z, 0.0f);
	Renderer::SetLight(light);

	ImGui::End();
}

// デバッグフリーカメラ（保留）
void CarDriveScene::debugFreeCamera()
{
	ImGui::Begin("debug Free camera");

	static float radius = 100.0f;
	static Vector3 pos = Vector3(0, 0, radius);
	static Vector3 lookat = Vector3(0, 0, 0);
	static float elevation = -90.0f * PI / 180.0f;
	static float azimuth = PI / 2.0f;

	static Vector3 spherecenter = Vector3(0, 0, 0);

	ImGui::SliderFloat("Radius", &radius, 1, 8000);
	ImGui::SliderFloat("Elevation", &elevation, -PI, PI);
	ImGui::SliderFloat("Azimuth", &azimuth, -PI, PI);

	ImGui::SliderFloat3("lookat ", &lookat.x, -100, 100);

	// カメラの位置を極座標からデカルト座標に変換
	m_camera.SetRadius(radius);
	m_camera.SetElevation(elevation);
	m_camera.SetAzimuth(azimuth);
	m_camera.SetLookat(lookat);

	// カメラの位置を極座標から求める
	m_camera.CalcCameraPosition();

	ImGui::End();
}

/**
 * @brief コンストラクタ
 */
CarDriveScene::CarDriveScene()
{
}

/**
 * @brief 更新処理
 *
 * @param deltatime 前フレームからの経過時間（ミリ秒）
 */
void CarDriveScene::update(uint64_t deltatime)
{

	m_player->update(deltatime);

	
	for (auto& enemy : m_enemies) {
		enemy->update(deltatime);
	}


	// ---------------------------
	MsgData posinfo;

	SRT srt = m_player->getSRT();
	posinfo.Msg.posinfobody.pos = srt.pos;
	posinfo.Msg.posinfobody.rotation = srt.rot;
	posinfo.Msg.posinfobody.scale = srt.scale;

	posinfo.Msg.Header.type = MessageType::POSITIONINFO;
	posinfo.Msg.Header.ID = m_myID;

	if (!m_yourip.empty() && m_yourport > 0) {
		m_udpcom->connectsub(m_yourip.c_str(), static_cast<uint16_t>(m_yourport));
		m_udpcom->sendcom(posinfo.data, sizeof(posinfo.data));
	}

	if (!m_yourip2.empty() && m_yourport2 > 0) {
		m_udpcom->connectsub(m_yourip2.c_str(), static_cast<uint16_t>(m_yourport2));
		m_udpcom->sendcom(posinfo.data, sizeof(posinfo.data));
	}
}

/**
 * @brief 描画処理
 *
 * @param deltatime 前フレームからの経過時間（ミリ秒）
 */
void CarDriveScene::draw(uint64_t deltatime)
{
	m_camera.Draw();

	// 3軸カラー
	Color axiscol[3] = {
		Color(1, 0, 0, 1),
		Color(0, 1, 0, 1),
		Color(0, 1, 1, 1)
	};

	// ワールド軸を描画
	SetLineWidth(1.0f);					// 太さを設定
	for (int axisno = 0; axisno < 3; axisno++)
	{
		Matrix4x4 rotmtx = Matrix4x4::Identity;
		m_segments[axisno]->Draw(rotmtx, axiscol[axisno]);
	}

	// フィールド描画
	m_field->draw(deltatime);

	
	m_player->draw(deltatime);

	
	for (auto& enemy : m_enemies) {
		enemy->draw(deltatime);
	}
}

/**
 * @brief シーンの初期化処理
 */
void CarDriveScene::init()
{
	// カメラ(3D)の初期化
	m_camera.Init();

	// ローカル軸表示用線分の初期化
	m_segments[0] = std::make_unique<Segment>(Vector3(0, 0, 0), Vector3(100, 0, 0));
	m_segments[1] = std::make_unique<Segment>(Vector3(0, 0, 0), Vector3(0, 100, 0));
	m_segments[2] = std::make_unique<Segment>(Vector3(0, 0, 0), Vector3(0, 0, 100));

	// 表示球の初期化
	SphereDrawerInit();

	// 表示線分の初期化
	LineDrawerInit();

	// メッシュデータ読み込み
	std::unique_ptr<CStaticMesh> staticmesh = std::make_unique<CStaticMesh>();
	staticmesh->Load("assets/model/car001.x", "assets/model/");

	std::unique_ptr<CStaticMeshRenderer> staticmeshrenderer = std::make_unique<CStaticMeshRenderer>();
	staticmeshrenderer->Init(*staticmesh);

	std::unique_ptr<CShader> shader = std::make_unique<CShader>();
	shader->Create("shader/vertexLightingVS.hlsl", "shader/vertexLightingPS.hlsl");

	// メッシュマネージャへ登録（メッシュ、使用するシェーダ、レンダラ）
	MeshManager::RegisterMesh<CStaticMesh>("car001.x", std::move(staticmesh));
	MeshManager::RegisterShader<CShader>("car001.x", std::move(shader));
	MeshManager::RegisterMeshRenderer<CStaticMeshRenderer>("car001.x", std::move(staticmeshrenderer));

	
	m_player = std::make_unique<player>();
	m_player->init();

	// フィールド
	m_field = std::make_unique<field>();
	m_field->init();

	// 亂數エンジンを初期化
	auto& rng = RandomEngine::tls();
	rng.uniformReal(-500, 500);

	
	for (int cnt = 0; cnt < static_cast<int>(ENEMYMAX); cnt++) {
		m_enemies[cnt] = std::make_unique<enemy>(this);
		m_enemies[cnt]->init();
		SRT srt;
		srt.scale = Vector3(1, 1, 1);
		srt.rot = Vector3(0, 0, 0);
		srt.pos = Vector3(
			static_cast<float>(rng.uniformReal(-500.0f, 500.0f)),
			0,
			static_cast<float>(rng.uniformReal(-500.0f, 500.0f)));

		m_enemies[cnt]->setSRT(srt);
	}

	// デバッグ Free Camera
	DebugUI::RedistDebugFunction([this]() {
		debugFreeCamera();
		});

	// ---------------------------
	// UDP 與 TOML：三選一（pia1/pia2/pia3）
	// ---------------------------
	m_udpcom = std::make_unique<UDPCOM>();

	std::cout << "Select pia1, pia2 or pia3" << std::endl;
	std::cout << " 0) pia1" << std::endl;
	std::cout << " 1) pia2" << std::endl;
	std::cout << " 2) pia3" << std::endl;

	int selectno = 0;
	std::cin >> selectno;

	std::string filename[3] = {
		"pia1/config.toml",
		"pia2/config.toml",
		"pia3/config.toml"
	};

	// Load config param
	auto config = toml::parse(filename[selectno]);

	
	m_myID = toml::find<int>(config, "myid");       // 0/1/2
	m_myport = toml::find<int>(config, "myport");

	
	m_yourport = toml::find<int>(config, "yourport1");
	m_yourip = toml::find<std::string>(config, "yourip1");

	
	m_yourport2 = toml::find<int>(config, "yourport2");
	m_yourip2 = toml::find<std::string>(config, "yourip2");

	std::cout << "myid   :" << m_myID << std::endl;
	std::cout << "myport :" << m_myport << std::endl;
	std::cout << "peer1  :" << m_yourip << ":" << m_yourport << std::endl;
	std::cout << "peer2  :" << m_yourip2 << ":" << m_yourport2 << std::endl;

	// UDP初期化
	m_udpcom->initcom(static_cast<uint16_t>(m_myport));
	m_recvthread = std::make_unique<recvThread>(m_udpcom.get());

	// 受信スレッド起動（位置封包）
	m_recvthread->registHandler(MessageType::POSITIONINFO,
		[this](std::unique_ptr<MsgData> msg, uint32_t ipadr, uint16_t port) {
			this->PositionInfoHandler(std::move(msg), ipadr, port);
		});

	// 受信スレッド開始
	m_recvthread->start();
}

/**
 * @brief シーンの終了処理
 */
void CarDriveScene::dispose()
{
	if (m_udpcom) m_udpcom->exitcom();
	if (m_recvthread) m_recvthread->join();
}
