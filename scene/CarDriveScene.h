#pragma once

#include	<array>
#include	<thread>

#include "../main.h"
#include "../system/camera.h"
#include "../system/IScene.h"
#include "../system/C3DShape.h"
#include "../system/SceneClassFactory.h"

#include "../gameobject/Player.h"
#include "../gameobject/enemy.h"
#include "../gameobject/field.h"

#include "../tool/RecvThread.h"

class UDPCOM;

class CarDriveScene : public IScene {
public:
	
	static constexpr uint32_t ENEMYMAX = 2;

	/// @brief コピーコンストラクタは使用不可
	CarDriveScene(const CarDriveScene&) = delete;

	/// @brief 代入演算子も使用不可
	CarDriveScene& operator=(const CarDriveScene&) = delete;

	/**
	 * @brief コンストラクタ
	 */
	explicit CarDriveScene();

	/**
	 * @brief 毎フレームの更新処理
	 * @param deltatime 前フレームからの経過時間（ミリ秒）
	 */
	void update(uint64_t deltatime) override;

	/**
	 * @brief 毎フレームの描画処理
	 * @param deltatime 前フレームからの経過時間（ミリ秒）
	 *
	 * 描画を行う。
	 */
	void draw(uint64_t deltatime) override;

	/**
	 * @brief シーンの初期化処理
	 */
	void init() override;

	/**
	 * @brief シーンの終了処理
	 */
	void dispose() override;

	/**
	 * @brief Directional Light
	 */
	void debugDirectionalLight();

	/**
	 * @brief Free Camera
	 */
	void debugFreeCamera();

	/**
	 * @brief get player
	 *
	 * get player object address
	 */
	player* getplayer() {
		return m_player.get();
	}

	
	void PositionInfoHandler(std::unique_ptr<MsgData> msg, uint32_t ipadr, uint16_t port);

private:
	FreeCamera m_camera;                                        // カメラ
	std::array<std::unique_ptr<Segment>, 3> m_segments;         // ワールド軸表示用線分

	std::unique_ptr<player>		m_player;                        // 本機玩家
	std::unique_ptr<field>		m_field;                         // フィールド
	std::array<std::unique_ptr<enemy>, ENEMYMAX>	 m_enemies;  // 遠端玩家鏡像 ×2

	std::unique_ptr<UDPCOM>		m_udpcom;                        // UDP通信クラス
	std::unique_ptr<recvThread> m_recvthread;                    // 受信スレッド


	int m_myID{};

	
	int m_myport{};                                             // 自分のポート番号
	int m_yourport{};                                           // 相手のポート番号（peer1）
	std::string m_yourip{};                                     // 相手のIPアドレス（peer1）

	std::string m_yourip2{};                                    // peer2 IP
	int m_yourport2{};                                          // peer2 Port
};

REGISTER_CLASS(CarDriveScene)