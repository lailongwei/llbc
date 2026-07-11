#pragma once

#include "llbc.h"
using namespace llbc;

class FuncTest_Com_Macro final : public LLBC_BaseTestCase
{
public:
    FuncTest_Com_Macro() = default;
    ~FuncTest_Com_Macro() override = default;

public:
    int Run(int argc, char *argv[]) override;
};
