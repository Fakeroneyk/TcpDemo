//
// Created by kaiyu on 2020/10/31.
//

#ifndef DEMO_DEMOSERVER_H
#define DEMO_DEMOSERVER_H

#include <cstdint>
#include <map>
#include "TcpConnection.h"
#include "singleton.h"
#include "MsgHead.h"
#include "Epoll.h"
#include "MsgHandler.h"
#include "Player.h"

struct PlayerInfo{
    int32_t fd;
    Player* player;
};

class DemoServer {
private:
    DemoServer(const DemoServer& player);
    DemoServer& operator = (const DemoServer& player);
public:
    DemoServer();
    ~DemoServer();


    int32_t initialize();

    int32_t recv_messages();

    int32_t working();

    void send_msg(int32_t uin, int32_t cmd_id, google::protobuf::Message &msg);
private:
    TCPSocket* m_ServerSocket = nullptr;
    stSocketInfo m_stSocketInfo[max_socket_count];
    std::map<int32_t, PlayerInfo> map_players;
    Epoll m_epoll;
    MsgHandler<Player>* m_msgHandle = nullptr;
private:
    int32_t process_code(const char *pszInCode, const int32_t iInCodeSize, int32_t socketfd);
    void register_all_messages();
};

#define DEMOSERVER Singleton<DemoServer>::Instance()
#endif //DEMO_DEMOSERVER_H
