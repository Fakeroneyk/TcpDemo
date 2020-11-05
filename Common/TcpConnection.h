//
// Created by kaiyu on 2020/10/27.
//

#ifndef DEMO_TCPCONNECTION_H
#define DEMO_TCPCONNECTION_H

#include <stdint.h>
#include <cstring>
#include <functional>
#include "base.h"
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888

enum {
    max_socket_count = 10240,
};

enum {
    max_ss_package_size = 0x200000,
    common_buffer_size	= 0x200000,	//2M大小，常规socket buffer缓存大小
};

enum enmConnErrorCode
{
    enmConnErrorCode_success					= 0,
    enmConnErrorCode_invalid_socket				= -1,
    enmConnErrorCode_recv_notenouth_buffer		= -2,
    enmConnErrorCode_peer_closed				= -3,
    enmConnErrorCode_send_notenouth_buffer		= -4,
    enmConnErrorCode_invalid_param				= -5,
    enmConnErrorCode_unknow						= -6,

};

class TCPSocket {
public:
    TCPSocket();
    ~TCPSocket();
private:
    TCPSocket(const TCPSocket& socket);
    TCPSocket& operator = (const TCPSocket& socket);
public:
    int32_t initialize(int32_t send_buffer_size, int32_t recv_buffer_size);
    void close_socket();

    int32_t recv_data();

    int32_t process_data(std::function<int32_t(const char *, const int32_t, int32_t)> callback);

    int32_t send_data(char* data, size_t size);

    int32_t get_one_code(char* data, size_t& size);

    int32_t get_fd();

    int32_t open_as_server(uint16_t port,char* ip = NULL);

    int32_t open_as_client(char* localIP = NULL, uint16_t localPort = 0, int32_t buffLen = -1);

    int32_t connect_to(u_long ip, uint16_t port, bool nonblock = true, int32_t msecond = 100);

    int32_t accept_fd(int32_t fd);

    void set_socket_fd(int32_t fd);
protected:
    int32_t m_socket_fd;

    //receiving buffer
    int32_t m_recv_head;
    int32_t m_recv_tail;
    char* m_recv_buffer;
    int32_t m_recv_buffer_size;
    char m_sRvMsgBuf[max_ss_package_size];
};

class TcpConnection {
public:
    TcpConnection();
    ~TcpConnection();

private:
    TcpConnection(const TcpConnection& con);
    TcpConnection& operator = (const TcpConnection& con);

public:
    int32_t initialize(uint64_t ip, uint16_t port, int32_t send_buffer_size = common_buffer_size, int32_t recv_buffer_size = common_buffer_size);

    TCPSocket& get_socket();

    int32_t connect_to(char* localIP = NULL, bool nonblock = true, int32_t msecond = 100, int32_t buffLen = -1);
protected:
    TCPSocket m_socket;
    uint64_t m_ip;
    uint16_t m_port;
};

struct stSocketInfo{
    TCPSocket* m_TcpSocket;
    int32_t m_socket_fd;
    enmFdType  m_fd_type;
};


#endif //DEMO_TCPCONNECTION_H
