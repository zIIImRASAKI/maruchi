#pragma once
#include	<WinSock2.h>
#include	<WS2tcpip.h>    
#include    <cinttypes>
#include    <string>
#include    <unordered_map>
#include	"../system/noncopyable.h"
#include    "../gameobject/gameobject.h"

// IPアドレスとポート番号とシーケンス番号を１つのキーとする為の構造体
struct MyKey {
    uint32_t ip{};
    uint16_t port{};
    uint16_t seq{};

    bool operator==(const MyKey& other) const {
        return ip == other.ip && port == other.port && seq == other.seq;
    }
    void SetIP(std::string ipstr) {
        inet_pton(AF_INET, ipstr.c_str(), &ip);
    }
    void SetIP(uint32_t ip) {
        this->ip = ip;
    }
    void SetPort(uint16_t port) {
        this->port = port;
    }
    void SetSeq(uint16_t seq) {
        this->seq = seq;
    }
};

// ハッシュ値を作るメソッドを用意
namespace std {
    template<>
    struct hash<MyKey> {
        uint64_t operator()(const MyKey& keydata) const {

            uint64_t key = keydata.ip;      // IPアドレスを64ビット変数に格納
            key <<= 32;                     // 32ビット左シフトしてポート番号のためのスペースを作る

            uint32_t port = keydata.port;
            port <<= 16;                    // 16ビット左シフトしてポート番号のためのスペースを作る
            key |= port;

            uint16_t seq = keydata.seq;
            key |= seq;                     // シーケンス番号を下位16ビットに格納

            return key;
        }
    };
}

// ネットワークオブジェクト検索クラス
class NetworkObjectSearch : NonCopyable {

    // 主キーとしてIPアドレスとポート番号とシーケンス番号を使う
    // データはゲームオブジェクトのポインタ
    inline static std::unordered_map<MyKey, gameobject*> m_NetworkObjectList;

public:
    // ネットワークオブジェクト登録
    static bool AddNetworkObject(std::string ip, uint16_t port, uint16_t seq, gameobject* obj) {
        MyKey key;
        key.SetIP(ip);
        key.SetPort(port);
        key.SetSeq(seq);

        // 既に登録されているかチェック
        if (m_NetworkObjectList.find(key) != m_NetworkObjectList.end()) {
            return false;
        }

        // 登録
        m_NetworkObjectList[key] = obj;
        return true;
    }

    // ネットワークオブジェクト登録
    static bool AddNetworkObject(uint32_t ip, uint16_t port, uint16_t seq, gameobject* obj) {
        MyKey key;
        key.SetIP(ip);
        key.SetPort(port);
        key.SetSeq(seq);

        // 既に登録されているかチェック
        if (m_NetworkObjectList.find(key) != m_NetworkObjectList.end()) {
            return false;
        }

        // 登録
        m_NetworkObjectList[key] = obj;
        return true;
    }


    // ネットワークオブジェクト削除
    static void EraseNetworkObject(std::string ip, uint16_t port, uint16_t seq) {
        MyKey key;
        key.SetIP(ip);
        key.SetPort(port);
        key.SetSeq(seq);

        // 既に登録されていれば削除
        if (m_NetworkObjectList.find(key) != m_NetworkObjectList.end()) {
            m_NetworkObjectList.erase(key);
        }
    }

    // ネットワークオブジェクト削除
    static void EraseNetworkObject(uint32_t ip, uint16_t port, uint16_t seq) {
        MyKey key;
        key.SetIP(ip);
        key.SetPort(port);
        key.SetSeq(seq);

        // 既に登録されていれば削除
        if (m_NetworkObjectList.find(key) != m_NetworkObjectList.end()) {
            m_NetworkObjectList.erase(key);
        }
    }


    // ネットワークオブジェクト全件削除
    static void ClearNetworkObjects() {
        m_NetworkObjectList.clear();
    }

    // ネットワークオブジェクト検索
    static gameobject* FindNetworkObject(std::string ip, uint16_t port, uint16_t seq) {
        MyKey key;
        key.SetIP(ip);
        key.SetPort(port);
        key.SetSeq(seq);

        // 検索
        if (m_NetworkObjectList.find(key) != m_NetworkObjectList.end()) {
            return m_NetworkObjectList[key];
        }
        return nullptr;
    }

    // ネットワークオブジェクト検索
    static gameobject* FindNetworkObject(uint32_t ip, uint16_t port, uint16_t seq) {
        MyKey key;
        key.SetIP(ip);
        key.SetPort(port);
        key.SetSeq(seq);

        // 検索
        if (m_NetworkObjectList.find(key) != m_NetworkObjectList.end()) {
            return m_NetworkObjectList[key];
        }
        return nullptr;
    }
};