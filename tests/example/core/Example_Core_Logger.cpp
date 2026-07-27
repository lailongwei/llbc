// The MIT License (MIT)

// Copyright (c) 2013 lailongwei<lailongwei@126.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), to deal in 
// the Software without restriction, including without limitation the rights to 
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
// the Software, and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "core/Example_Core_Logger.h"

int Example_Core_Logger::Run(int argc, char *argv[])
{
    // Logging uses the LLOG_XXX family of macros (printf-style formatting).
    // When no log config file is loaded, logs are output directly to the console via the root logger,
    // which is the simplest logging approach that runs with zero configuration.

    // ========================= 1. Log at each level =========================
    // Levels from low to high: Trace < Debug < Info < Warn < Error < Fatal.
    // Note: since v1.1.1, Trace is the lowest level (replacing the old lowest, Debug).
    LLBC_PrintLn("Log at all levels (via root logger):");
    LLOG_TRACE("this is a TRACE log");
    LLOG_DEBUG("this is a DEBUG log");
    LLOG_INFO("this is an INFO log");
    LLOG_WARN("this is a WARN log");
    LLOG_ERROR("this is an ERROR log");
    LLOG_FATAL("this is a FATAL log");

    // ========================= 2. Format arguments =========================
    // The LLOG_XXX macros support printf-style variadic formatting.
    const char *player = "Judy";
    int level = 99;
    double hp = 87.5;
    LLOG_INFO("player=%s, level=%d, hp=%.1f", player, level, hp);

    // ========================= 3. Logs with a tag =========================
    // LLOG_XXX3(tag, fmt, ...) attaches a business tag to the log, for easier classification and lookup.
    LLOG_INFO3("LOGIN", "player %s logged in", player);

    return LLBC_OK;
}
