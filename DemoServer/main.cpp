//
// Created by kaiyu on 2020/10/27.
//

#include <cstring>
#include <stdint.h>
#include <signal.h>
#include <cstdio>
#include <map>
#include "base.h"
#include "DemoServer.h"

int main(int argc, char** argv){
    if(success == DEMOSERVER.initialize()) {
        DEMOSERVER.working();
    }
    return 0;
}