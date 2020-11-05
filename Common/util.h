//
// Created by kaiyu on 2020/10/27.
//

#ifndef DEMO_UTIL_H
#define DEMO_UTIL_H

#include <stdint.h>
#include <string>
#include "base.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t ip_string_to_addr(const char* string_ip, int32_t& ip);
int32_t encode_int32(char **pOut,  uint32_t Src);
int32_t decode_int32(char **pIn,  uint32_t *pOut);
int32_t set_non_block( int32_t fd );
int32_t connect_nonblock(int fd, struct sockaddr_in* serv_addr, socklen_t addrlen, int32_t msecond);
bool is_num(std::string type);


#ifdef __cplusplus
}
#endif


#endif //DEMO_UTIL_H
