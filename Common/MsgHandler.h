//
// Created by kaiyu on 2020/10/29.
//

#ifndef DEMO_MSGHANDLER_H
#define DEMO_MSGHANDLER_H

#include "stdint.h"
#include "MsgHead.h"
#include <map>


template <class T>
class MsgHandler {
public:
    typedef int32_t (T::*funHandle) (MsgHead& stHead, const char* body, const int32_t len);
private:
    std::map<int32_t, funHandle> m_mapRequest;
public:
    void RegisterMsg(int32_t MessageID, funHandle func){
        if (m_mapRequest.find(MessageID) != m_mapRequest.end()) {
            printf("[MsgHandler::%s] conflict client msg id(id:%d)\n", __FUNCTION__, MessageID);
            exit(0);
        }
        m_mapRequest[MessageID] = func;
    }
    funHandle get_func(int32_t MessageID){
        auto it = m_mapRequest.find(MessageID);
        if ( it != m_mapRequest.end()) {
            return it->second;
        }
        printf("[MsgHandler::%s] iMessageID=(%d | 0x%x) is not register\n", __FUNCTION__, MessageID,MessageID);
        return NULL;
    }
};

#endif //DEMO_MSGHANDLER_H
