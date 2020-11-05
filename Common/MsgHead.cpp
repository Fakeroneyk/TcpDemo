//
// Created by kaiyu on 2020/10/27.
//

#include "base.h"
#include "MsgHead.h"
#include "util.h"

MsgHead::MsgHead() {
    m_iPackageLen = 0;
    m_iMessageID  = -1;
    m_iUin        = 0;
}

MsgHead::~MsgHead() {

}

MsgHead::MsgHead(MsgHead& head) {
    m_iPackageLen           = head.m_iPackageLen;
    m_iUin                  = head.m_iUin;
    m_iMessageID            = head.m_iMessageID;
}

int32_t MsgHead::encode(char* pszOut, int32_t& iOutLength) {
    if (NULL == pszOut) {
        return false;
    }

    char* ptmp = pszOut;
    int32_t coded_length = 0;

    iOutLength = 0;

    coded_length = encode_int32(&ptmp, (uint32_t)m_iPackageLen);
    iOutLength += coded_length;

    coded_length = encode_int32(&ptmp, (uint32_t)m_iUin);
    iOutLength += coded_length;

    coded_length = encode_int32(&ptmp, (uint32_t)m_iMessageID);
    iOutLength += coded_length;

    return true;
}

int32_t MsgHead::decode(const char* pszIn, const int32_t iInLength) {
    if (NULL == pszIn || iInLength <= 0) {
        return fail;
    }

    char* ptmp = const_cast<char*>(pszIn);
    int32_t remained_length = iInLength;
    int32_t decoded_length = 0;

    decoded_length = decode_int32(&ptmp, (uint32_t*)(&m_iPackageLen));
    remained_length -= decoded_length;

    decoded_length = decode_int32(&ptmp, (uint32_t*)(&m_iUin));
    remained_length -= decoded_length;

    decoded_length = decode_int32(&ptmp, (uint32_t*)(&m_iMessageID));
    remained_length -= decoded_length;

    if (remained_length < 0) {
        return fail;
    }

    return success;
}