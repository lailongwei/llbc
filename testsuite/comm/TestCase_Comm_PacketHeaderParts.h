/**
 * @file    TestCase_Comm_PacketHeaderParts.h
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2015/10/01
 * @version 1.0
 *
 * @brief
 */
#ifndef __LLBC_TEST_CASE_COMM_PACKET_HEADER_PARTS_H__
#define __LLBC_TEST_CASE_COMM_PACKET_HEADER_PARTS_H__

#include "llbc.h"
using namespace llbc;

class TestCase_Comm_PacketHeaderParts : public LLBC_BaseTestCase
{
public:
    TestCase_Comm_PacketHeaderParts();
    virtual ~TestCase_Comm_PacketHeaderParts();

public:
    virtual int Run(int argc, char *argv[]);

private:
    template <typename T>
    void TestOne(int serialNo, const T &val, const char *typeDesc);

    void TestSetToPacket();

private:
    LLBC_PacketHeaderParts _parts;
};

#endif // !__LLBC_TEST_CASE_COMM_PACKET_HEADER_PARTS_H__
