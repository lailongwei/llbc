#pragma once

#include "llbc.h"
using namespace llbc;

class TestCase_Com_Macro : public LLBC_BaseTestCase
{
public:
    TestCase_Com_Macro() = default;
    ~TestCase_Com_Macro() override = default;

public:
    int Run(int argc, char *argv[]) override;
};
