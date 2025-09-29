#pragma once
#define WIN32_LEAN_AND_MEAN
#define _WINSOCKAPI_

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include	<winsock2.h>				// こちらが先
#include	<windows.h>
#include	<stdio.h>
#include	"wsock32error.h"

#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR, 12)

class UDPCOM{
private:
	WSADATA		m_wd{};					// ＷＳＡＳＴＡＲＴＵＰ用
	u_short		m_myportno{};				// ポート番号
	SOCKET		m_mysock{};				// ソケット番号
	sockaddr_in	m_myaddr{};				// 自分のアドレス
	sockaddr_in m_toaddr{};					// 送信先アドレス
	sockaddr_in m_fromaddr{};				// 送信元アドレス

	bool		m_alreadyinit{};			// 初期化完了フラグ
	bool		m_connectok{};				// 送信先アドレスセット済みフラグ
public:
	UDPCOM(){
		m_alreadyinit = false;
		m_connectok = false;
	}

	//////////////////////////////////////////
	//	ソケットエラーメッセージ表示処理	//
	//////////////////////////////////////////
	void	errcom();

	//////////////////////////
	//	ソケットの初期処理  //
	//////////////////////////
	void initcom(int useportno);

	//////////////////////////////
	//	ソケット通信終了処理    //
	//////////////////////////////
	void exitcom(void);

	///////////////////
	//	UDP送信処理  //
	///////////////////
	void sendcom(char* sendchar, int senddatasize);

	/////////////////////////////
	//	送信先アドレス設定処理 //
	/////////////////////////////
	bool connectsub(const char* inip, u_short portno);

	/////////////////////////////
	//	送信元情報を取得する 　//
	/////////////////////////////
	sockaddr_in getfromaddr();
		
	/////////////////////////////
	//	受信処理 　　　　　　　//
	/////////////////////////////
	int recv(char* rcvbuf, int size);
	int recv(char* rcvbuf, int size, sockaddr_in& fromaddr);

};