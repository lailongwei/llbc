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
    sint64 sint64Val;
    LLBC_String strVal;

    void Serialize(LLBC_Stream &stream) const
    {
        LLBC_PrintLn("Serialize_Test2::Serialize() method call");
        LLBC_STREAM_BEGIN_WRITE(stream);

        LLBC_STREAM_WRITE(sint64Val);
        LLBC_STREAM_WRITE(strVal);

        LLBC_STREAM_END_WRITE();
    }

    bool DeSerialize(LLBC_Stream &stream)
    {
        LLBC_PrintLn("Serializ_Test2::DeSerialize() method call");
        LLBC_STREAM_BEGIN_READ(stream, bool, false);

        LLBC_STREAM_READ(sint64Val);
        LLBC_STREAM_READ(strVal);

        LLBC_STREAM_END_READ_RET(true);
    }
};

struct Serialize_Test3
{
    sint64 sint64Val;
    std::string strVal;

    void serialize(LLBC_Stream &stream) const
    {
        LLBC_PrintLn("Serialize_Test3::serialize() method call");
        stream <<sint64Val
               <<strVal;
    }

    bool deserialize(LLBC_Stream &stream)
    {
        LLBC_PrintLn("Serialize_Test3::deserialize() method call");
        stream >>sint64Val
               >>strVal;
        return true;
    }
};

static void ClearTest();
static void RawSerializeTest();
static void STLContainersSerializeTest();
static void MethodSerializeTest();
static void ReplaceTest();

static LLBC_String ToStringVec(const std::vector<int> &vec);
static LLBC_String ToStringNestingVec(const std::vector<std::vector<int> > &vec);

static LLBC_String ToStringSet(const std::set<int> &s);

static LLBC_String ToStringMap(const std::map<int, int> &m);

int TestCase_Com_Stream::Run(int argc, char *argv[])
{
    ClearTest();
    RawSerializeTest();
    STLContainersSerializeTest();
    MethodSerializeTest();
    ReplaceTest();

    LLBC_PrintLn("Press any key to continue ...");
    getchar();

    return LLBC_OK;
}

static void ClearTest()
{
    LLBC_PrintLn("Clear stream test:");
    LLBC_Stream s;
    s.Write(32);
    s.Write(true);
    s.Write(false);
    LLBC_PrintLn("Before clear non-attach stream, s.pos:%lu, s.cap:%lu, s.buf:%p, attached:%d", s.GetPos(), s.GetCap(), s.GetBuf(), s.IsAttach());

    s.Clear();
    LLBC_PrintLn("After clear non-attack stream, s.pos:%lu, s.cap:%lu, s.buf:%p, attached:%d", s.GetPos(), s.GetCap(), s.GetBuf(), s.IsAttach());

    LLBC_Stream s2;
    char *buf = LLBC_Malloc(char, 1024);
    s2.Attach(buf, 1024);
    s2.Write(10086);
    s2.Skip(1000);
    LLBC_PrintLn("Before clear attach stream, s.pos:%lu, s.cap:%lu, s.buf:%p, attached:%d", s2.GetPos(), s2.GetCap(), s2.GetBuf(), s2.IsAttach());

    s2.Clear();
    LLBC_PrintLn("After clear attach stream, s.pos:%lu, s.cap:%lu, s.buf:%p, attached:%d", s2.GetPos(), s2.GetCap(), s2.GetBuf(), s2.IsAttach());

    free(buf);
}

static void RawSerializeTest()
{
    Serialize_Test test1 = {8, 16, 32, 64};
    LLBC_PrintLn("before serialize, "
        "sint8Val: %d, sint16val: %d, sint32val: %d, sint64val: %llu",
        test1.sint8Val, test1.sint16Val, test1.sint32Val, test1.sint64Val);

    LLBC_Stream stream;
    stream.Write(test1);
    
    Serialize_Test test2;
    stream.SetPos(0);
    stream.Read(test2);
    LLBC_PrintLn("after deserialize, "
        "sint8Val: %d, sint16val: %d, sint32val: %d, sint64val: %llu",
        test2.sint8Val, test2.sint16Val, test2.sint32Val, test2.sint64Val);
}

static void STLContainersSerializeTest()
{
    // std::string test.
    std::string str = "STL std::string";
    LLBC_PrintLn("before serialize std::string: %s", str.c_str());

    LLBC_Stream stream;
    stream.Write(str);

    std::string str2;
    stream.SetPos(0);
    stream.Read(str2);
    LLBC_PrintLn("after deserialize std::string: %s", str2.c_str());

    // std::vector<int> test.
    std::vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    LLBC_PrintLn("before serialize std::vector<int>: %s", ToStringVec(vec).c_str());  

    stream.SetPos(0);
    stream.Write(vec);

    std::vector<int> vec2;
    stream.SetPos(0);
    stream.Read(vec2);
    LLBC_PrintLn("after deserialize std::vector<int>: %s", ToStringVec(vec2).c_str());

    // std::vector<std::vector<int> > test.
    std::vector<std::vector<int> > nestingVec;
    nestingVec.push_back(std::vector<int>());
    nestingVec.push_back(std::vector<int>());
    nestingVec.push_back(std::vector<int>());
    for (size_t i = 0; i < nestingVec.size(); ++i)
    {
        for (int j = 0; j < 3; ++j)
            nestingVec[i].push_back(j + 1);
    }
    LLBC_PrintLn("before serialize std::vector<std::vector<int> >: %s", ToStringNestingVec(nestingVec).c_str());

    stream.SetPos(0);
    stream.Write(nestingVec);

    std::vector<std::vector<int> > nestingVec2;
    stream.SetPos(0);
    stream.Read(nestingVec2);
    LLBC_PrintLn("after deserialize std::vector<std::vector<int> >: %s", ToStringNestingVec(nestingVec2).c_str());

    // std::set<int> test.
    std::set<int> s;
    s.insert(1);
    s.insert(2);
    s.insert(3);
    LLBC_PrintLn("before serialize std::set<int>: %s", ToStringSet(s).c_str());

    stream.SetPos(0);
    stream.Write(s);

    std::set<int> s2;
    stream.SetPos(0);
    stream.Read(s2);
    LLBC_PrintLn("after deserialize std::set<int>: %s", ToStringSet(s2).c_str());

    // std::map<int, int> test.
    std::map<int, int> m;
    m.insert(std::make_pair(1, 10));
    m.insert(std::make_pair(2, 20));
    m.insert(std::make_pair(3, 30));
    LLBC_PrintLn("before serialize std::map<int, int>: %s", ToStringMap(m).c_str());

    stream.SetPos(0);
    stream.Write(m);
    
    std::map<int, int> m2;
    stream.SetPos(0);
    stream.Read(m2);
    LLBC_PrintLn("after deserialize std::map<int, int>: %s", ToStringMap(m2).c_str());
}

static void MethodSerializeTest()
{
    LLBC_PrintLn("Method serialize test...");

    // Serialize/DeSerialize
    LLBC_Stream stream;
    Serialize_Test2 test2_1 = {64, "hello"};
    stream <<test2_1;
    LLBC_PrintLn("- test2_1 serialized(Serialize()), "
        "val:[int64Val:%llu, strVal:%s], stream pos:%lu",
        test2_1.sint64Val, test2_1.strVal.c_str(), stream.GetPos());

    Serialize_Test2 test2_2;
    stream.SetPos(0);
    stream >> test2_2;
    LLBC_PrintLn("- stream deserialized(Deserialize()), "
        "val:[int64Val:%llu, strVal:%s], stream pos:%lu",
        test2_2.sint64Val, test2_2.strVal.c_str(), stream.GetPos());

    // serialize/deserialize
    Serialize_Test3 test3_1 = {128, "world"};
    stream.SetPos(0);
    stream <<test3_1;
    LLBC_PrintLn("- test3_1 serialized(serialize()), "
        "val:[int64Val:%llu, strVal:%s], stream pos:%lu",
        test3_1.sint64Val, test3_1.strVal.c_str(), stream.GetPos());

    Serialize_Test3 test3_2;
    stream.SetPos(0);
    stream >> test3_2;
    LLBC_PrintLn("- stream deserialized(Deserialize()), "
        "val:[int64Val:%llu, strVal:%s], stream pos:%lu",
        test3_2.sint64Val, test3_2.strVal.c_str(), stream.GetPos());

    LLBC_PrintLn("Method serialize test finished!");
}

static void ReplaceTest()
{
    LLBC_PrintLn("Replace test...");

    LLBC_Stream stream;
    stream.Write("Hello world\0", 12);
    LLBC_PrintLn("  - Before replace, pos:%lu, buf:%s", stream.GetPos(), stream.GetBuf<char>());

    LLBC_PrintLn("  - Insert 'Hey ' after space character...");
    stream.Replace(6, 6, "Hey ", 4);
    LLBC_PrintLn("  - After insert, pos:%lu, buf:%s", stream.GetPos(), (stream.GetBuf<char>()));

    LLBC_PrintLn("  - Replace 'Hey' to 'ABC'...");
    stream.Replace(6, 9, "ABC", 3);
    LLBC_PrintLn("  - After replace, pos:%lu, buf:%s", stream.GetPos(), (stream.GetBuf<char>()));

    LLBC_PrintLn("  - Erase 'ABC ' from stream...");
    stream.Replace(6, 10, nullptr, 0);
    LLBC_PrintLn("  - After erase, pos:%lu, buf:%s", stream.GetPos(), (stream.GetBuf<char>()));

    LLBC_PrintLn("  - Erase '\\0' termermal character...");
    stream.Replace(stream.GetPos() - 1, stream.npos, nullptr, 0);

    char appendStr[] = ", Hello World Too";
    LLBC_PrintLn("  - Append '%s' to stream...", appendStr);
    stream.Replace(LLBC_Stream::npos, LLBC_Stream::npos, appendStr, sizeof(appendStr));
    LLBC_PrintLn("  - After append '%s', pos:%lu, buf:%s",
                   appendStr,
                   stream.GetPos(),
                   (stream.GetBuf<char>()));
}

static LLBC_String ToStringVec(const std::vector<int> &vec)
{
    LLBC_String out;

    out.append_format("%d|", vec.size());
    if (vec.empty())
    {
        out.append("<no elems>");
    }
    else
    {
        for (size_t i = 0; i < vec.size(); ++i)
        {
            out.append_format("%d", vec[i]);
            if (i != vec.size() - 1)
                out.append(",");
        }
    }

    return out;
}

static LLBC_String ToStringNestingVec(const std::vector<std::vector<int> > &vec)
{
    LLBC_String out;
    if (vec.empty())
    {
        out.append("{0|<no elems>}");
    }
    else
    {
        out.append_format("{%d|", vec.size());
        for (size_t i = 0; i < vec.size(); ++i)
        {
            out.append_format("{%s}", ToStringVec(vec[i]).c_str());
            if (i != vec.size() - 1)
                out.append(",");
        }

        out.append("}");
    }

    return out;
}

static LLBC_String ToStringSet(const std::set<int> &s)
{
    LLBC_String out;

    out.append_format("{%d|", s.size());
    if (s.empty())
    {
        out.append("<no elems>");
    }
    else
    {
        std::set<int>::const_iterator iter = s.begin();
        for (; iter != s.end(); )
        {
            out.append_format("%d", *iter);
            if (++iter != s.end())
                out.append(",");
        }
    }

    out.append("}");

    return out;
}

LLBC_String ToStringMap(const std::map<int, int> &m)
{
    LLBC_String out;

    out.append_format("{%d|", m.size());
    if(m.empty())
    {
        out.append("<no elems>");
    }
    else
    {
        std::map<int, int>::const_iterator iter = m.begin();
        for (; iter != m.end(); )
        {
            out.append_format("[%d->%d]", iter->first, iter->second);
            if(++iter != m.end())
                out.append(",");
        }
    }

    out.append("}");

    return out;
}
