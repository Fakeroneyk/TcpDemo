//
// Created by kaiyu on 2020/10/28.
//

#include "DemoClient.h"
#include "util.h"
#include "base.h"
#include "Epoll.h"
#include "MsgHead.h"
DemoClient::DemoClient() {
    m_ClientSocket = new TcpConnection();
    m_msgHandle = new MsgHandler<DemoClient>();
}

DemoClient::~DemoClient() {
    delete m_ClientSocket;
    delete m_msgHandle;
}

int32_t DemoClient::initialize() {
    int32_t server_ip = 0;
    int32_t ret = ip_string_to_addr(SERVER_IP,server_ip);
    if(ret != 0) {
        printf("DemoClient::%s failed\n",__FUNCTION__ );
        return fail;
    }
    m_ClientSocket->initialize(server_ip,SERVER_PORT);

    ret = m_ClientSocket->connect_to();
    if(success == ret){
        m_stSocketInfo[m_ClientSocket->get_socket().get_fd()].m_TcpSocket = &m_ClientSocket->get_socket();
        m_stSocketInfo[m_ClientSocket->get_socket().get_fd()].m_socket_fd = fd_type_socket;
        m_stSocketInfo[m_ClientSocket->get_socket().get_fd()].m_fd_type = fd_type_socket;

    }
    else{
        printf("DemoClient::%s connect_to failed\n",__FUNCTION__ );
        return fail;
    }

    if(m_epoll.epoll_init(max_socket_count) < 0){
        printf("epoll_init failed\n");
        return fail;
    }

    if(m_epoll.epoll_add(m_ClientSocket->get_socket().get_fd()) < 0){
        printf("m_epoll.epoll_add fd:%d failed\n",m_ClientSocket->get_socket().get_fd());
        return fail;
    }

    if((ret = m_epoll.epoll_add(STDIN_FILENO)) < 0){
        printf("m_epoll.epoll_add fd:%d failed ret = %d  errno=%d\n",STDIN_FILENO,ret,errno);
        return fail;
    }
    m_stSocketInfo[STDIN_FILENO].m_socket_fd = STDIN_FILENO;
    m_stSocketInfo[STDIN_FILENO].m_fd_type = fd_type_stdin;

    register_all_messages();
    return success;
}

int32_t DemoClient::working() {
    for(;;) {
        int32_t timeout = 1;
        int32_t fd_count = m_epoll.epoll_wait(timeout);

        for (int32_t i = 0; i < fd_count; i++) {
            struct epoll_event *pstEvent = m_epoll.get_event(i);
            int32_t socketfd = pstEvent->data.fd;
            enmFdType type = m_stSocketInfo[socketfd].m_fd_type;
            switch (type) {
                case fd_type_stdin: {
                    del_io_msg();
                }
                    break;
                case fd_type_socket: {
                    recv_messages();
                }
                    break;
                default:
                    printf("error fd_type %d\n", (int32_t) type);
                    break;
            }
        }
    }
    return success;
}

int32_t DemoClient::recv_messages() {
    int ret = m_ClientSocket->get_socket().recv_data();
    if (ret < 0) {
        printf("[DemoClient::%s] recv_data failed errorcode:%d\n",
               __FUNCTION__, ret);
        return fail;
    }
    while (true) {
        size_t data_size = max_ss_package_size;
        ret = m_ClientSocket->get_socket().get_one_code(m_sRvMsgBuf, data_size);
        if (ret > 0) {
            process_code(m_sRvMsgBuf, data_size);
            continue;
        } else if (ret < 0) {
            printf("[DemoClient::%s] get_one_code failed. socket:%d errorCode:%d\n",
                   __FUNCTION__, m_ClientSocket->get_socket().get_fd(), ret);
        }
        break;
    }
    return 0;
}

int32_t DemoClient::del_io_msg() {
    static char buffer[common_buffer_size];
    int ret = read(STDIN_FILENO,buffer,sizeof(buffer));
    std::string type(buffer,ret);
    if(!is_num(type)){
        printf("please input a number!\n");
        return fail;
    }
    int32_t typeno = stoi(type);

    switch (typeno){
        case 1:
        {
            send_msg_req_test1();
        }
        break;
        case 2:
        {
            send_msg_req_test2();
        }
        break;
        default:
            break;
    }
    return 0;
}

void DemoClient::send_msg_req_test1() {
    ReqTest req;
    req.set_a(123);

    send_msg(22, DEMOID::REQ_TEST, req);
}

void DemoClient::send_msg_req_test2() {
    ReqTest2 req;
    req.set_aa(321);

    send_msg(22, DEMOID::REQ_TEST2, req);
}

void DemoClient::send_msg(int32_t uin, int32_t cmd_id, google::protobuf::Message &msg) {
    static char data[max_ss_package_size];
    MsgHead head;
    head.m_iMessageID = cmd_id;
    head.m_iPackageLen = MESSAGE_HEAD_SIZE + msg.ByteSize();
    head.m_iUin = uin;

    int32_t coded_length = 0;
    head.encode(data,coded_length);
    msg.SerializePartialToArray(data+coded_length,msg.ByteSize());
    int32_t fd = m_ClientSocket->get_socket().get_fd();
    int32_t ret = m_ClientSocket->get_socket().send_data(data,(size_t)head.m_iPackageLen);
    if(ret < success){
        printf("send msg (id=%d) error ret=%d,errno:%d ,strerror:%s,fd = %d\n",cmd_id,ret,errno, strerror(errno),fd);
    }
    if(ret > success){
        printf("send msg (id=%d)  try multi ret=%d, errno:%d ,strerror:%s, fd = %d\n",cmd_id,ret,errno, strerror(errno),fd);
    }
    printf("send msg  fd:%d    msglen = %d\n",fd,head.m_iPackageLen);
}

int32_t DemoClient::process_code(const char *pszInCode, const int32_t iInCodeSize) {
    if (!pszInCode || iInCodeSize <= 0) {
        return fail;
    }
    MsgHead stHead;
    int32_t head_outLength = MESSAGE_HEAD_SIZE;
    if (success != stHead.decode(pszInCode, iInCodeSize)) {
        printf("[DemoClient::%s]head decode error\n", __FUNCTION__);
        return fail;
    }
    if (stHead.m_iPackageLen != iInCodeSize) {
        printf("[DemoClient::%s]the package size decoded from service was not equal with received size.\n", __FUNCTION__);
        return fail;
    }

    int32_t iBodySize = stHead.m_iPackageLen - head_outLength;

    printf("[network][DemoClient::%s][uin:%d][msg_id:%d]\n", __FUNCTION__, stHead.m_iUin, stHead.m_iMessageID);

    auto func = m_msgHandle->get_func(stHead.m_iMessageID);
    if(NULL != func){
        (this->*func)(stHead,pszInCode + head_outLength,iBodySize);
    }
    return success;
}

int32_t DemoClient::handle_test(MsgHead &stHead, const char *body, const int32_t len) {
    RspTest rsp;
    rsp.ParseFromArray(body, len);
    printf("client recv msg  rsp.b:%d\n",rsp.b());
    return 0;
}

int32_t DemoClient::handle_test2(MsgHead &stHead, const char *body, const int32_t len) {
    return 0;
}

void DemoClient::register_all_messages() {
    m_msgHandle->RegisterMsg(DEMOID::RSP_TEST, &DemoClient::handle_test);
}
