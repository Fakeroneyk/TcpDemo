//
// Created by kaiyu on 2020/10/27.
//

#include "TcpConnection.h"
#include "base.h"
#include "util.h"
#include <netinet/tcp.h>
#include <cstring>
#include <cstdio>

TCPSocket::TCPSocket() {
    m_recv_head = 0;
    m_recv_tail = 0;
    m_recv_buffer_size = common_buffer_size;
    m_recv_buffer = NULL;
}

TCPSocket::~TCPSocket() {
    close_socket();
    if (NULL != m_recv_buffer) {
        delete [] m_recv_buffer;
        m_recv_buffer = NULL;
    }
    m_recv_head = 0;
    m_recv_tail = 0;
}

int32_t TCPSocket::get_fd() {
    int32_t fd = -1;
    fd = m_socket_fd;
    return fd;
}

int32_t TCPSocket::initialize(int32_t send_buffer_size, int32_t recv_buffer_size) {
    if (NULL == m_recv_buffer || recv_buffer_size != m_recv_buffer_size){
        m_recv_buffer_size = recv_buffer_size;
        if (NULL != m_recv_buffer){
            delete [] m_recv_buffer;
            m_recv_buffer = NULL;
        }
        m_recv_buffer = new char [m_recv_buffer_size+1];
        memset(m_recv_buffer, 0, sizeof(char)*(m_recv_buffer_size+1));
    }
    m_recv_head = 0;
    m_recv_tail = 0;
    return success;
}

void TCPSocket::close_socket() {
    if (m_socket_fd < 0){
        return;
    }

    close(m_socket_fd);
    m_socket_fd = -1;
}

int32_t TCPSocket::recv_data() {
    int ret = 0;
    if (m_socket_fd < 0){
        return (int32_t)enmConnErrorCode_invalid_socket;
    }

    if(m_recv_tail == m_recv_head){
        m_recv_head = 0;
        m_recv_tail = 0;
    }

    int32_t received_byte = 0;

    do {
        if(m_recv_tail == m_recv_buffer_size){
            if(m_recv_head > 0){
                //注意: 要使用memmove而不是memcpy
                //memcpy的__restrict__关键字不允许内存重叠
                memmove(&m_recv_buffer[0], &m_recv_buffer[m_recv_head], size_t(m_recv_tail - m_recv_head));
                m_recv_tail -= m_recv_head;
                m_recv_head = 0;
            }
            else{
                ret = (int32_t)enmConnErrorCode_recv_notenouth_buffer;
                break;
            }
        }

        received_byte = recv(m_socket_fd, &m_recv_buffer[m_recv_tail], size_t(m_recv_buffer_size - m_recv_tail), 0);
        //printf("recvived_byte=%d\n",received_byte);
        if (received_byte > 0){
            m_recv_tail += received_byte;
        }
        else if (0 == received_byte){
            close_socket();
            ret = enmConnErrorCode_peer_closed;
            break;
        }
        else if (EAGAIN != errno) {//received_byte < 0 && EAGAIN != errno
            printf("errno:%d\n",errno);
            close_socket();
            ret = enmConnErrorCode_unknow;
            break;
        }
    }while(received_byte > 0);

    return ret;
}

int32_t TCPSocket::send_data(char *data, size_t size) {
    int32_t ret = 0;
    if (NULL == data || 0 == size){
        return (int32_t)enmConnErrorCode_invalid_param;
    }

    if (m_socket_fd < 0){
        return (int32_t)enmConnErrorCode_invalid_socket;
    }

    int32_t remainded = size;
    int32_t sended = 0;
    int32_t nTime = 0;

    char* pszTmp = data;

    while(remainded > 0){
        sended = send(m_socket_fd, pszTmp, (size_t)remainded, 0);
        if (sended > 0){
            pszTmp += sended;
            remainded -= sended;
        }
        else{
            if (sended == 0 || (sended < 0 && EAGAIN != errno && EINTR != errno)){
                close_socket();
                ret = (int32_t)enmConnErrorCode_unknow;
                if (sended == 0)ret = enmConnErrorCode_peer_closed;
                break;
            }
        }

        ++nTime;
    }

    if (nTime > 1 && remainded == 0){	//全部传输完，并且传输次数超过两次
        return nTime;//mean "try multi"
    }

    return ret;
}

int32_t TCPSocket::get_one_code(char *data, size_t &size) {
    if (NULL == data){
        return -1;
    }
    int32_t buffer_data_size = 0;
    buffer_data_size = m_recv_tail - m_recv_head;
    //判断接收缓冲区内的数据大小
    if (buffer_data_size <= 0){
        return 0;
    }

    //根据表示长度的字节数，检查数据的合法性
    if (buffer_data_size < (int32_t)sizeof(int32_t)){
        if (m_recv_tail == m_recv_buffer_size){
            memcpy(&m_recv_buffer[0], &m_recv_buffer[m_recv_head], size_t(buffer_data_size));
            m_recv_head = 0;
            m_recv_tail = buffer_data_size;

        }
        return 0;
    }

    //长度字段占用4byte
    int32_t code_size = (int32_t)ntohl((u_long) (*(int32_t*)&m_recv_buffer[m_recv_head]));

    if (code_size <= 0){
        return -2;
    }

    if (code_size >= common_buffer_size){
        size = (size_t)code_size;
        return -3;
    }

    //若接收缓冲区内的数据不足一个code
    if (buffer_data_size < code_size){
        //并且数据已经存放到缓冲区尾了, 则移动数据到接收缓冲区头部
        //if (m_recv_tail == (int32_t)sizeof(m_recv_buffer))
        if (m_recv_tail == m_recv_buffer_size){
            memmove(&m_recv_buffer[0], &m_recv_buffer[m_recv_head], size_t(buffer_data_size));
            m_recv_head = 0;
            m_recv_tail = buffer_data_size;
        }
        return 0;
    }

    if((int32_t)size < code_size){
        return -4;
    }

    size = (size_t)code_size;
    memcpy(data, &m_recv_buffer[m_recv_head], size);

    m_recv_head += code_size;
    if (m_recv_tail == m_recv_head){
        m_recv_head = 0;
        m_recv_tail = 0;
    }

    return 1;
}

int32_t TCPSocket::open_as_server(uint16_t port, char *ip) {
    if(m_socket_fd > 0){
        close_socket();
    }
    m_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket_fd < 0){
        printf("open socket failed, m_socket_fd < 0, with code[%d]\n", errno);
        m_socket_fd = -1;
        return -1;
    }

    int flags = 1;
    struct linger ling = {0, 0};
    setsockopt(m_socket_fd, SOL_SOCKET, SO_KEEPALIVE, &flags, sizeof(flags));
    setsockopt(m_socket_fd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));

    setsockopt(m_socket_fd, IPPROTO_TCP, TCP_NODELAY, &flags, sizeof(flags)); //set TCP_CORK

    if (0 != setsockopt(m_socket_fd, SOL_SOCKET, SO_REUSEADDR, &flags, (int)sizeof(flags))){
        printf("setsockopt failed with code[%d]\n", errno);
        close_socket();
        return -1;
    }

    struct sockaddr_in stSocketAddr;
    memset(&stSocketAddr, 0x0, sizeof(stSocketAddr));
    stSocketAddr.sin_family = AF_INET;
    if (NULL != ip){
        stSocketAddr.sin_addr.s_addr = inet_addr(ip);
    }
    else{
        stSocketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }

    stSocketAddr.sin_port = (u_short)htons(port);
    socklen_t addrSize = socklen_t(sizeof(stSocketAddr));
    if (0 != bind(m_socket_fd, (const sockaddr*) &stSocketAddr, addrSize)){
        printf("bind failed with code[%d]\n", errno);
        close_socket();
        return -1;
    }

    if (0 != listen(m_socket_fd, 128)){
        printf("listen failed with code[%d]\n", errno);
        close_socket();
        return -1;
    }

    //设置为非阻塞
    set_non_block(m_socket_fd);
    return 0;
}

int32_t TCPSocket::open_as_client(char *localIP, uint16_t localPort, int32_t buffLen) {
    //open socket
    m_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket_fd < 0)
    {
        m_socket_fd = -1;
        return -1;
    }

    struct sockaddr_in stLocalAddress;
    memset(&stLocalAddress, 0x0, sizeof(stLocalAddress));

    stLocalAddress.sin_family = AF_INET;
    if(NULL != localIP){
        stLocalAddress.sin_addr.s_addr = inet_addr(localIP);
    }
    else {
        stLocalAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    if(0 != localPort){
        stLocalAddress.sin_port = htons(localPort);
    }
    int32_t iOptValue = 1;
    if (0 != setsockopt(m_socket_fd, SOL_SOCKET, SO_REUSEADDR, &iOptValue, sizeof(iOptValue))){
        close_socket();
        return -1;
    }
    setsockopt(m_socket_fd, IPPROTO_TCP, TCP_NODELAY, &iOptValue, sizeof(iOptValue)); //set TCP_CORK

    if(buffLen != -1){
        if (0 != setsockopt(m_socket_fd, SOL_SOCKET, SO_SNDBUF, (const void *)&buffLen, sizeof(buffLen))){
            return -1;
        }

        if (0 != setsockopt(m_socket_fd, SOL_SOCKET, SO_RCVBUF, (const void *)&buffLen, sizeof(buffLen))){
            return -1;
        }
    }

    socklen_t addressSize = (socklen_t)sizeof(stLocalAddress);
    if (0 != bind(m_socket_fd, (const struct sockaddr*) &stLocalAddress, addressSize)){
        close_socket();
        return -1;
    }

    m_recv_head = 0;
    m_recv_tail = 0;

    return 0;
}

int32_t TCPSocket::connect_to(u_long ip, uint16_t port, bool nonblock, int32_t msecond) {
    if (0 > m_socket_fd)
    {
        return -1;
    }

    struct sockaddr_in stDstAddress;
    memset(&stDstAddress, 0x0, sizeof(stDstAddress));
    stDstAddress.sin_family = AF_INET;
    stDstAddress.sin_addr.s_addr = ip;
    stDstAddress.sin_port = htons(port);
    socklen_t sockSize = (socklen_t)sizeof(stDstAddress);

    if (0 != connect_nonblock(m_socket_fd, &stDstAddress, sockSize, msecond))
    {
        close_socket();
        return -1;
    }

    if (nonblock)
    {
        set_non_block(m_socket_fd);
    }

    m_recv_head = 0;
    m_recv_tail = 0;

    return 0;
}

void TCPSocket::set_socket_fd(int32_t fd) {
    m_socket_fd = fd;
}

int32_t TCPSocket::accept_fd(int32_t fd) {
    int32_t accepted_sockfd = -1;
    struct sockaddr_in stSocketAddress;
    socklen_t sockaddress_len = (socklen_t)sizeof(stSocketAddress);
    accepted_sockfd = accept(fd, (struct sockaddr *)&stSocketAddress, &sockaddress_len);
    if(0 >= accepted_sockfd)
    {
        printf(" accept client(%s:%d) error code = %d, msg = %s\n",
               inet_ntoa(stSocketAddress.sin_addr), stSocketAddress.sin_port,
               errno, strerror(errno));
        return -1;
    }

    //设置为非阻塞socket
    int flags = 1;
    if ( ioctl(accepted_sockfd, FIONBIO, &flags) &&  ((flags = fcntl(accepted_sockfd, F_GETFL, 0)) < 0
                                                      ||  fcntl(accepted_sockfd, F_SETFL, flags | O_NONBLOCK) < 0) )
    {
        printf("set accepted socket O_NONBLOCK failed, just close it!\n");
        close(accepted_sockfd);
        return -1;
    }

    //开启TCP_NODELAY
    int on = 1;
    setsockopt(accepted_sockfd, IPPROTO_TCP, TCP_NODELAY, (const void*)&on, sizeof(on));
    return accepted_sockfd;
}

int32_t TCPSocket::process_data(std::function<int32_t(const char *, const int32_t, int32_t)> callback) {
    int ret = recv_data();

    if (ret < 0) {
        printf("[TCPSocket::%s] recv_data failed fd:%d errorcode:%d\n",
               __FUNCTION__, m_socket_fd, ret);
        return fail;
    }
    while (true) {
        size_t data_size = max_ss_package_size;
        ret = get_one_code(m_sRvMsgBuf, data_size);
        if (ret > 0) {
            callback(m_sRvMsgBuf, data_size, m_socket_fd);
            continue;
        } else if (ret < 0) {
            printf("[TCPSocket::%s] get_one_code failed. socket:%d errorCode:%d\n",
                   __FUNCTION__, m_socket_fd, ret);
        }
        break;
    }
    return success;
}

TcpConnection::TcpConnection() {

}

TcpConnection::~TcpConnection() {

}

int32_t TcpConnection::initialize(uint64_t ip, uint16_t port, int32_t send_buffer_size, int32_t recv_buffer_size) {
    m_ip = ip;
    m_port = port;
    m_socket.initialize(send_buffer_size,recv_buffer_size);
    return 0;
}

TCPSocket& TcpConnection::get_socket() {
    return m_socket;
}

int32_t TcpConnection::connect_to(char *localIP, bool nonblock, int32_t msecond, int32_t buffLen) {
    int32_t ret = m_socket.open_as_client(localIP,0,buffLen);
    if(0 != ret){
        return -1;
    }

    ret = m_socket.connect_to(m_ip, m_port, nonblock, msecond);
    if (0 != ret)
    {
        return -1;
    }

    return ret;
}