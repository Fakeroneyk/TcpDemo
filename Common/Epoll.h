//
// Created by kaiyu on 2020/10/27.
//

#ifndef DEMO_EPOLL_H
#define DEMO_EPOLL_H

#include <stdint.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdlib.h>

class Epoll {
public:
    Epoll();
    ~Epoll();
    int32_t epoll_init(int size);
    int32_t epoll_wait(int32_t timeout = -1);
    int32_t epoll_add(int32_t);
    struct epoll_event* get_event(int32_t);

private:
    int32_t m_size;
    int32_t m_epoll_fd;
    struct epoll_event* m_pevents;
    struct epoll_event m_epoll_event;
};


#endif //DEMO_EPOLL_H
