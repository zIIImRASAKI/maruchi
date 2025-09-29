#include    "../tool/UDPCom.h"
#include	"RecvThread.h"

void recvThread::registHandler(MessageType type, std::function<void(std::unique_ptr<MsgData>, uint32_t, uint16_t)> handler) 
{
	// メッセージディスパッチャ(メッセージタイプ毎に処理関数を振り分ける)
	m_msgdispacher.registerHandler(type, handler);
}

// 受信スレッド
void recvThread::recvthread()
{

	while (1) {
		// 受信バッファを毎回生成（次の受信で書き換えられないようにするため）
		std::unique_ptr<MsgData> recvbuf = std::make_unique<MsgData>();
		sockaddr_in fromaddr{};

		memset(recvbuf->data, 0, sizeof(MsgData));
		int recvsize = m_udpcom->recv(recvbuf->data, sizeof(MsgData), fromaddr);

		if (recvsize == SOCKET_ERROR) {
			// エラー処理	
			m_udpcom->errcom();
			break;
		}

		// 受信したメッセージを該当処理に渡す
		m_msgdispacher.dispatch(
			std::move(recvbuf),									// 受信バッファ
			fromaddr.sin_addr.S_un.S_addr,						// 送信元IPアドレス
			fromaddr.sin_port);									// 送信元ポート番号
	}
}

void recvThread::start() 
{
	// this ポインタとメンバ関数ポインタを渡す
	m_thread = std::thread(&recvThread::recvthread, this);
}

void recvThread::join() {
	m_thread.join();
}
