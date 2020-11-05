//
// Created by kaiyu on 2020/10/27.
//

#ifndef DEMO_MSGHEAD_H
#define DEMO_MSGHEAD_H

#include <cstdint>

#define MESSAGE_HEAD_SIZE	( 3 * sizeof(int32_t) )
class MsgHead {
public:
    MsgHead();
    ~MsgHead();
    MsgHead(MsgHead& head);

    int32_t encode(char* pszOut,int32_t& iOutLength);
    int32_t decode(const char* pszIn, const int32_t iInLength);

    int32_t m_iPackageLen;
    int32_t m_iUin;
    int32_t m_iMessageID;
};

#endif //DEMO_MSGHEAD_H
