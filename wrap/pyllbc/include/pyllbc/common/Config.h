/*
 * @Author: your name
 * @Date: 2022-01-10 10:29:52
 * @LastEditTime: 2022-03-03 12:43:08
 * @LastEditors: Please set LastEditors
 * @Description: 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \llbc\wrap\pyllbc\include\pyllbc\common\Config.h
 */
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

#ifndef __PYLLBC_COM_CONFIG_H__
#define __PYLLBC_COM_CONFIG_H__

// Define task count limit, default is 16.
#define PYLLBC_CFG_TASK_LMT                                     16

// The service default CODEC config.
//      0: Json Codec
//      1: Binary Codec
#define PYLLBC_CFG_DFT_SVC_CODEC                                1

// Packet reuse option.
#define PYLLBC_CFG_PACKET_REUSE                                 1

// Service frame-timeout function switch, if enabled, the service will has frame-timeout limit function, default is 0.
#define PYLLBC_CFG_COMM_ENABLE_SERVICE_FRAME_TIMEOUT            1

#endif // !__PYLLBC_COM_CONFIG_H__
