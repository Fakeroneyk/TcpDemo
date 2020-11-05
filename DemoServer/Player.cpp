//
// Created by kaiyu on 2020/10/27.
//

#include "base.h"
#include <cstdio>
#include "Player.h"
#include "DemoServer.h"

Player::Player(int32_t uin) {
    m_uin = uin;
}

Player::~Player() {

}

int32_t Player::uin(){
    return m_uin;
}

int32_t Player::handle_test(MsgHead &stHead, const char *body, const int32_t len) {
    ReqTest req;
    req.ParseFromArray(body, len);

    printf("1111111111111111111111  req.a:%d\n",req.a());

    RspTest rsp;
    rsp.set_b(321);
    send_msg(DEMOID::RSP_TEST, rsp);
    return success;
}

int32_t Player::handle_test2(MsgHead &stHead, const char *body, const int32_t len) {
    ReqTest2 req;
    req.ParseFromArray(body, len);

    printf("2222222222222222222222  req.aa:%d\n",req.aa());
    return success;
}

void Player::send_msg(int32_t cmd_id, google::protobuf::Message &msg) {
    DEMOSERVER.send_msg(uin(),cmd_id,msg);
}


