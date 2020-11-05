//
// Created by kaiyu on 2020/10/28.
//

#ifndef DEMO_DEMOCLIENT_H
#define DEMO_DEMOCLIENT_H

#include <cstdint>
#include "TcpConnection.h"
#include "singleton.h"
#include "MsgHead.h"
#include "base.h"
#include "MsgHandler.h"
#include "Epoll.h"

class DemoClient {
private:
    DemoClient(const DemoClient& demoClient);
    DemoClient& operator = (const DemoClient& demoClient);
public:
    DemoClient();
    ~DemoClient();

    int32_t initialize();

    int32_t working();

    int32_t recv_messages();

    void send_msg(int32_t uin, int32_t cmd_id, google::protobuf::Message &msg);

    int32_t handle_test(MsgHead& stHead, const char* body, const int32_t len);
    int32_t handle_test2(MsgHead& stHead, const char* body, const int32_t len);
private:
    Epoll m_epoll;
    TcpConnection * m_ClientSocket = nullptr;
    stSocketInfo m_stSocketInfo[max_socket_count];

    char m_sRvMsgBuf[max_ss_package_size];
    MsgHandler<DemoClient>* m_msgHandle = nullptr;
private:
    int32_t process_code(const char *pszInCode, const int32_t iInCodeSize);
    int32_t del_io_msg();
    void send_msg_req_test1();
    void send_msg_req_test2();
    void register_all_messages();

};
#define DEMOCLIENT Singleton<DemoClient>::Instance()

#endif //DEMO_DEMOCLIENT_H
