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


#include "common/TestCase_Com_Stream.h"

#pragma pack(push, 1)
struct Serialize_Test
{
    sint8 sint8Val;
    sint16 sint16Val;
    sint32 sint32Val;
    sint64 sint64Val;
};
#pragma pack(pop)

struct Serialize_Test2
{
    sint64 sint64val;
    LLBC_String strVal;

    void Serialize(LLBC_Stream &stream) const
    {
        LLBC_PrintLine("Serialize() method call");
        LLBC_STREAM_BEGIN_WRITE(stream);

        LLBC_STREAM_WRITE(sint64val);
        LLBC_STREAM_WRITE(strVal);

        LLBC_STREAM_END_WRITE();
    }

    bool DeSerialize(LLBC_Stream &stream)
    {
        LLBC_PrintLine("DeSerialize() method call");
        LLBC_STREAM_BEGIN_READ(stream, bool, false);

        LLBC_STREAM_READ(sint64val);
        LLBC_STREAM_READ(strVal);

        LLBC_STREAM_END_READ_RET(true);
    }
};

static void RawSerializeTest();
static void STLContainersSerializeTest();
static void MethodSerializeTest();

static LLBC_String ToStringVec(const std::vector<int> &vec);
static LLBC_String ToStringNestingVec(const std::vector<std::vector<int> > &vec);

static LLBC_String ToStringSet(const std::set<int> &s);

static LLBC_String ToStringMap(const std::map<int, int> &m);

int TestCase_Com_Stream::Run(int argc, char *argv[])
{
    RawSerializeTest();
    STLContainersSerializeTest();
    MethodSerializeTest();

    LLBC_PrintLine("Press any key to continue ...");
    getchar();

    return LLBC_OK;
}

static void RawSerializeTest()
{
    Serialize_Test test1 = {8, 16, 32, 64};
    LLBC_PrintLine("before serialize, "
        "sint8Val: %d, sint16val: %d, sint32val: %d, sint64val: %llu",
        test1.sint8Val, test1.sint16Val, test1.sint32Val, test1.sint64Val);

    LLBC_Stream stream;
    stream.Write(test1);
    
    Serialize_Test test2;
    stream.SetPos(0);
    stream.Read(test2);
    LLBC_PrintLine("after deserialize, "
        "sint8Val: %d, sint16val: %d, sint32val: %d, sint64val: %llu",
        test2.sint8Val, test2.sint16Val, test2.sint32Val, test2.sint64Val);
}

static void STLContainersSerializeTest()
{
    // std::string test.
    std::string str = "STL std::string";
    LLBC_PrintLine("before serialize std::string: %s", str.c_str() );

    LLBC_Stream stream;
    stream.Write(str);
    LLBC_Stream streamEx;
    streamEx.WriteEx(str);

    std::string str2;
    stream.SetPos(0);
    stream.Read(str2);
    LLBC_PrintLine("after deserialize std::string: %s", str2.c_str() );
    streamEx.SetPos(0);
    streamEx.ReadEx(str2);
    LLBC_PrintLine("after deserializeex std::string: %s", str2.c_str() );

    // std::vector<int> test.
    std::vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    LLBC_PrintLine("before serialize std::vector<int>: %s", ToStringVec(vec).c_str() );  

    stream.SetPos(0);
    stream.Write(vec);
    streamEx.SetPos(0);
    streamEx.WriteEx(vec);

    std::vector<int> vec2;
    stream.SetPos(0);
    stream.Read(vec2);
    LLBC_PrintLine("after deserialize std::vector<int>: %s", ToStringVec(vec2).c_str() );
    streamEx.SetPos(0);
    streamEx.ReadEx(vec2);
    LLBC_PrintLine("after deserializeex std::vector<int>: %s", ToStringVec(vec2).c_str() );

    // std::vector<std::vector<int> > test.
    std::vector<std::vector<int> > nestingVec;
    nestingVec.push_back( std::vector<int>() );
    nestingVec.push_back( std::vector<int>() );
    nestingVec.push_back( std::vector<int>() );
    for(size_t i = 0; i < nestingVec.size(); i ++)
    {
        for(int j = 0; j < 3; j ++)
        {
            nestingVec[i].push_back(j + 1);
        }
    }
    LLBC_PrintLine("before serialize std::vector<std::vector<int> >: %s", ToStringNestingVec(nestingVec).c_str() );

    stream.SetPos(0);
    stream.Write(nestingVec);
    streamEx.SetPos(0);
    streamEx.WriteEx(nestingVec);

    std::vector<std::vector<int> > nestingVec2;
    stream.SetPos(0);
    stream.Read(nestingVec2);
    LLBC_PrintLine("after deserialize std::vector<std::vector<int> >: %s", ToStringNestingVec(nestingVec2).c_str() );
    streamEx.SetPos(0);
    streamEx.ReadEx(nestingVec2);
    LLBC_PrintLine("after deserializeex std::vector<std::vector<int> >: %s", ToStringNestingVec(nestingVec2).c_str() );

#if LLBC_TARGET_PLATFORM_NON_WIN32
    // std::set<int> test.
    std::set<int> s;
    s.insert(1);
    s.insert(2);
    s.insert(3);
    LLBC_PrintLine("before serialize std::set<int>: %s", ToStringSet(s).c_str() );

    stream.SetPos(0);
    stream.Write(s);
    streamEx.SetPos(0);
    streamEx.WriteEx(s);

    std::set<int> s2;
    stream.SetPos(0);
    stream.Read(s2);
    LLBC_PrintLine("after deserialize std::set<int>: %s", ToStringSet(s2).c_str() );
    streamEx.SetPos(0);
    streamEx.ReadEx(s2);
    LLBC_PrintLine("after deserializeex std::set<int>: %s", ToStringSet(s2).c_str() );
#else // LLBC_TARGET_PLATFORM_WIN32
    LLBC_PrintLine("In WIN32 platform, not support std::set serialize/deserailzie!");
#endif // LLBC_TARGET_PLATFORM_NON_WIN32

#if LLBC_TARGET_PLATFORM_NON_WIN32
    // std::map<int, int> test.
    std::map<int, int> m;
    m.insert( std::make_pair(1, 10) );
    m.insert( std::make_pair(2, 20) );
    m.insert( std::make_pair(3, 30) );
    LLBC_PrintLine("before serialize std::map<int, int>: %s", ToStringMap(m).c_str() );

    stream.SetPos(0);
    stream.Write(m);
    streamEx.SetPos(0);
    streamEx.WriteEx(m);
    
    std::map<int, int> m2;
    stream.SetPos(0);
    stream.Read(m2);
    LLBC_PrintLine("after deserialize std::map<int, int>: %s", ToStringMap(m2).c_str() );
    streamEx.SetPos(0);
    streamEx.ReadEx(m2);
    LLBC_PrintLine("after deserializeex std::map<int, int>: %s", ToStringMap(m2).c_str() );
#else // LLBC_TARGET_PLATFORM_WIN32
    LLBC_PrintLine("In WIN32 platform, Stream not support serialize/deserialize pair type STL container");
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
}

static void MethodSerializeTest()
{
    Serialize_Test2 test1 = {64, "hello"};
    LLBC_PrintLine("before serialize, "
        "sint64val: %lld, strVal: %s", test1.sint64val, test1.strVal.c_str() );

    LLBC_Stream stream;
    stream.Write(test1);

    Serialize_Test2 test2;
    stream.SetPos(0);
    stream.Read(test2);
    LLBC_PrintLine("after deserialize, "
        "sint64Val: %lld, strVal: %s\n", test2.sint64val, test2.strVal.c_str() );
}

static LLBC_String ToStringVec(const std::vector<int> &vec)
{
    LLBC_String out;

    out.append_format( "%d|", vec.size() );
    if( vec.empty() )
    {
        out.append("<no elems>");
    }
    else
    {
        for(size_t i = 0; i < vec.size(); i ++)
        {
            out.append_format("%d", vec[i]);
            if(i != vec.size() - 1)
            {
                out.append(",");
            }
        }
    }

    return out;
}

static LLBC_String ToStringNestingVec(const std::vector<std::vector<int> > &vec)
{
    LLBC_String out;
    if( vec.empty() )
    {
        out.append("{0|<no elems>}");
    }
    else
    {
        out.append_format( "{%d|", vec.size() );
        for(size_t i = 0; i < vec.size(); i ++)
        {
            out.append_format( "{%s}", ToStringVec(vec[i]).c_str() );
            if(i != vec.size() - 1)
            {
                out.append(",");
            }
        }

        out.append("}");
    }

    return out;
}

static LLBC_String ToStringSet(const std::set<int> &s)
{
    LLBC_String out;

    out.append_format( "{%d|", s.size() );
    if( s.empty() )
    {
        out.append("<no elems>");
    }
    else
    {
        std::set<int>::const_iterator iter = s.begin();
        for(; iter != s.end(); )
        {
            out.append_format("%d", *iter);
            if( ++ iter != s.end() )
            {
                out.append(",");
            }
        }
    }

    out.append("}");

    return out;
}

LLBC_String ToStringMap(const std::map<int, int> &m)
{
    LLBC_String out;

    out.append_format( "{%d|", m.size() );
    if( m.empty() )
    {
        out.append("<no elems>");
    }
    else
    {
        std::map<int, int>::const_iterator iter = m.begin();
        for(; iter != m.end(); )
        {
            out.append_format("[%d->%d]", iter->first, iter->second);
            if( ++ iter != m.end() )
            {
                out.append(",");
            }
        }
    }

    out.append("}");

    return out;
}
