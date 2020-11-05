//
// Created by kaiyu on 2020/10/27.
//

#ifndef DEMO_BASE_H
#define DEMO_BASE_H

#if defined(WIN32) || defined(_WIN32)
#include <winsock2.h>
#include <sys/types.h>
#include <sys/timeb.h>
#else
#include <signal.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <dirent.h>
#include <dlfcn.h>
#include <sys/select.h>
#include <assert.h>
#include <iostream>
#include <string>

#endif

#include "../Proto/ProtoID.pb.h"
#include "../Proto/Demo.pb.h"
enum {
    success = 0,
    fail = 1,
};

enum enmFdType{
    fd_type_invalid = 0,
    fd_type_stdin   = 1,
    fd_type_socket  = 2,
    fd_type_listen  = 3,
};


#endif //DEMO_BASE_H
