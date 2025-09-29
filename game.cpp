#include	<iostream>
#include	"system/renderer.h"
#include    "system/DebugUI.h"
#include    "system/CDirectInput.h"
#include	"system/scenemanager.h"
#include	"fpscontrol.h"

void gameinit() 
{
	// レンダラの初期化    
	Renderer::Init();

	// DirectInputの初期化
	CDirectInput::GetInstance().Init(Application::GetHInstance(), 
		Application::GetWindow(),
		Application::GetWidth(),
		Application::GetHeight());

	// デバッグUIの初期化
	DebugUI::Init(Renderer::GetDevice(), Renderer::GetDeviceContext());

	// シーンマネージャの初期化
	SceneManager::Init();

	//　シーン選択
	SceneManager::SetCurrentScene("CarDriveScene");

}

// 単位はマイクロ秒
void gameupdate(uint64_t deltatime)
{
	CDirectInput::GetInstance().GetKeyBuffer();		// キーボードの状態を取得
	CDirectInput::GetInstance().GetMouseState();	// マウスの状態を取得

	// シーンマネージャの更新
	SceneManager::Update(deltatime);
}

// 単位はマイクロ秒
void gamedraw(uint64_t deltatime) 
{
	// レンダリング前処理
	Renderer::Begin();

	// シーンマネージャの描画
	SceneManager::Draw(deltatime);

	// デバッグUIの描画
	DebugUI::Render();

	// レンダリング後処理
	Renderer::End();
}

void gamedispose() 
{
	// デバッグUIの終了処理
	DebugUI::DisposeUI();

	// シーンマネージャの終了処理
	SceneManager::Dispose();

	// レンダラの終了処理
	Renderer::Uninit();

}

void gameloop()
{
	static FPS fpsrate(65);

	// 初回のみ基準確定
	static bool first = true;
	if (first) { fpsrate.Tick(); first = false; }

	// 前フレームの実Δt（sleep込み）
	uint64_t dt = fpsrate.GetDeltaUS();			// マイクロ秒で取得

	dt = dt / 1000;								// ミリ秒に変換

	gameupdate(dt);
	gamedraw(dt);

	// 次フレームまで待機しつつ、次Δtを確定
	fpsrate.Tick();
}