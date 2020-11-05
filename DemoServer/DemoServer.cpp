//
// Created by kaiyu on 2020/10/31.
//

#include "DemoServer.h"

DemoServer::DemoServer() {
    m_ServerSocket = new TCPSocket();
    m_msgHandle    = new MsgHandler<Player>();
}

DemoServer::~DemoServer() {
    delete m_ServerSocket;
    delete m_msgHandle;
}

int32_t DemoServer::initialize() {
    m_ServerSocket->initialize(common_buffer_size,common_buffer_size);
    if(m_ServerSocket->open_as_server(SERVER_PORT) < 0){
        printf("m_ServerSocket->open_as_server failed\n");
        return fail;
    }
    if(m_epoll.epoll_init(max_socket_count) < 0){
        printf("epoll_init failed\n");
        return fail;
    }
    if(m_epoll.epoll_add(m_ServerSocket->get_fd()) < 0){
        printf("m_epoll.epoll_add fd:%d failed\n",m_ServerSocket->get_fd());
        return fail;
    }
    m_stSocketInfo[m_ServerSocket->get_fd()].m_TcpSocket  = m_ServerSocket;
    m_stSocketInfo[m_ServerSocket->get_fd()].m_socket_fd  = m_ServerSocket->get_fd();
    m_stSocketInfo[m_ServerSocket->get_fd()].m_fd_type    = fd_type_listen;

    register_all_messages();
    return success;
}

int32_t DemoServer::process_code(const char *pszInCode, const int32_t iInCodeSize,int32_t socketfd){
    if (!pszInCode || iInCodeSize <= 0) {
        return fail;
    }
    MsgHead stHead;
    int32_t head_outLength = MESSAGE_HEAD_SIZE;
    if (success != stHead.decode(pszInCode, iInCodeSize)) {
        printf("[Player::%s]head decode error\n", __FUNCTION__);
        return fail;
    }
    if (stHead.m_iPackageLen != iInCodeSize) {
        printf("[Player::%s]the package size decoded from service was not equal with received size.\n", __FUNCTION__);
        return fail;
    }

    int32_t iBodySize = stHead.m_iPackageLen - head_outLength;

    printf("[network][Player::%s][uin:%d][msg_id:%d]\n", __FUNCTION__, stHead.m_iUin, stHead.m_iMessageID);
    if(map_players.find(stHead.m_iUin) == map_players.end()){
        map_players[stHead.m_iUin].fd = socketfd;
        map_players[stHead.m_iUin].player = new Player(stHead.m_iUin);
    }
    auto func = m_msgHandle->get_func(stHead.m_iMessageID);
    if(NULL != func){
        (map_players[stHead.m_iUin].player->*func)(stHead,pszInCode + head_outLength,iBodySize);
    }
    return success;
}

int32_t DemoServer::recv_messages() {
    int32_t timeout = 1;
    int32_t fd_count = m_epoll.epoll_wait(timeout);
    for(int32_t i = 0; i < fd_count; i++){
        struct epoll_event* pstEvent = m_epoll.get_event(i);
        int32_t socketfd = pstEvent->data.fd;
        TCPSocket* pstSocket = m_stSocketInfo[socketfd].m_TcpSocket;
        if(pstSocket == NULL || pstSocket->get_fd() < 0){
            printf("[Player::%s] get_server_tcpsocket failed fd:%d\n", __FUNCTION__, socketfd);
            return fail;
        }

        if(m_stSocketInfo[socketfd].m_fd_type == fd_type_listen){
            int32_t accepted_sockfd = pstSocket->accept_fd(socketfd);
            if(accepted_sockfd < 0){
                printf("accept_fd  failed\n");
                continue;
            }
            m_stSocketInfo[accepted_sockfd].m_TcpSocket  = new TCPSocket();
            m_stSocketInfo[accepted_sockfd].m_TcpSocket->initialize(common_buffer_size,common_buffer_size);
            m_stSocketInfo[accepted_sockfd].m_TcpSocket->set_socket_fd(accepted_sockfd);
            m_stSocketInfo[accepted_sockfd].m_socket_fd  = accepted_sockfd;
            m_stSocketInfo[accepted_sockfd].m_fd_type    = fd_type_socket;

            if(m_epoll.epoll_add(accepted_sockfd) < 0){
                printf("m_epoll.epoll_add fd:%d failed\n",m_ServerSocket->get_fd());
                return fail;
            }
        }
        else if(m_stSocketInfo[socketfd].m_fd_type == fd_type_socket) {
            int ret = pstSocket->process_data(std::bind(&DemoServer::process_code, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
            if(fail == ret){
                printf("socket process_data failed fd:%d\n",pstSocket->get_fd());
            }
        }
    }
    return success;
}

int32_t DemoServer::working() {
    for(;;){
        recv_messages();
    }
    return success;
}

void DemoServer::send_msg(int32_t uin, int32_t cmd_id, google::protobuf::Message &msg) {
    static char data[max_ss_package_size];
    MsgHead head;
    head.m_iMessageID = cmd_id;
    head.m_iPackageLen = MESSAGE_HEAD_SIZE + msg.ByteSize();
    head.m_iUin = uin;

    int32_t coded_length = 0;
    head.encode(data,coded_length);
    msg.SerializePartialToArray(data+coded_length,msg.ByteSize());
    if(map_players.find(uin) == map_players.end()){
        printf("no uin in server uin:%d\n",uin);
        return;
    }
    int32_t fd = map_players[uin].fd;
    int32_t ret = m_stSocketInfo[fd].m_TcpSocket->send_data(data,(size_t)head.m_iPackageLen);
    if(ret < success){
        printf("send msg (id=%d) error ret=%d,errno:%d ,strerror:%s,fd = %d\n",cmd_id,ret,errno, strerror(errno),fd);
    }
    if(ret > success){
        printf("send msg (id=%d)  try multi ret=%d, errno:%d ,strerror:%s, fd = %d\n",cmd_id,ret,errno, strerror(errno),fd);
    }
    printf("send msg  fd:%d    msglen = %d\n",fd,head.m_iPackageLen);
}

void DemoServer::register_all_messages() {
    m_msgHandle->RegisterMsg(DEMOID::REQ_TEST, &Player::handle_test);
    m_msgHandle->RegisterMsg(DEMOID::REQ_TEST2, &Player::handle_test2);
}
