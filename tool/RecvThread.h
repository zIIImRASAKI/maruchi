#pragma once
#include	<thread>

class UDPCOM;

#include    "../system/noncopyable.h"
#include	"CMessageDispacher.h"
#include	"message.h"

class recvThread : NonCopyable {
	UDPCOM* m_udpcom{};
	CMessageDispacher m_msgdispacher{};
	std::thread		m_thread{};
public:

	recvThread() = delete;

	recvThread(UDPCOM* udpcomobj) {
		m_udpcom = udpcomobj;
	}

	void registHandler(MessageType type, std::function<void(std::unique_ptr<MsgData>, uint32_t, uint16_t)> handler);

	// 受信スレッド
	void recvthread();

	// 受信スレッド開始
	void start();

	// 終了同期
	void join();

};