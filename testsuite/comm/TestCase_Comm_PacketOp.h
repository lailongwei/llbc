/**
 * @file    TestCase_Comm_PacketOp.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2014/03/08
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_COMM_PACKET_OP_H__
#define __LLBC_TEST_CASE_COMM_PACKET_OP_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Comm_PacketOp : public LLBC_BaseTestCase
{
public:
    TestCase_Comm_PacketOp();
    virtual ~TestCase_Comm_PacketOp();

public:
    virtual int Run(int argc, char *argv[]);
};

#endif // !__LLBC_TEST_CASE_COMM_PACKET_OP_H__
