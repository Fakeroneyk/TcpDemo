//
// Created by kaiyu on 2020/10/27.
//

#ifndef DEMO_PLAYER_H
#define DEMO_PLAYER_H

#include <cstdint>
#include "TcpConnection.h"
#include "singleton.h"
#include "MsgHead.h"
#include "Epoll.h"
#include "MsgHandler.h"
#include <map>

class Player {
public:
    Player(int32_t uin);
    ~Player();

    int32_t uin();

public:
    int32_t handle_test(MsgHead& stHead, const char* body, const int32_t len);
    int32_t handle_test2(MsgHead& stHead, const char* body, const int32_t len);
private:
    int32_t m_uin;
private:
    void send_msg(int32_t cmd_id, google::protobuf::Message &msg);
};



#endif //DEMO_PLAYER_H
