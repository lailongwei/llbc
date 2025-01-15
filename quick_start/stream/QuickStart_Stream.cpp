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

#include "stream/QuickStart_Stream.h"

// ����Ա�enum
enum class PlayerSex
{
    Male,
    Female,
};

// �������, ��ͨ��������̽��������л�
struct PlayerData
{
    // ���Id
    uint64 playerId;
    // �����
    LLBC_String playerName;

    // ��ҵȼ�
    int level;
    // ����Ա�
    PlayerSex sex;

    // ToString
    LLBC_String ToString() const
    {
        return LLBC_String().format(
            "PlayerData[%llu, name:%s, level:%d, sex:%s]",
            playerId,
            playerName.c_str(),
            level,
            sex == PlayerSex::Female ? "Female" : "Male");
    }

    // ���л�֧��
    void Serialize(LLBC_Stream &stream) const
    {
        stream << playerId
               << playerName
               << level
               << sex;
    }

    // �����л�֧��
    bool Deserialize(LLBC_Stream &stream)
    {
        stream >> playerId
               >> playerName
               >> level
               >> sex;

        return true;
    }
};

int QuickStart_Stream::Run(int argc, char *argv[])
{
    // ��ʼ��llbc���
    LLBC_Startup();
    // Defer����llbc���
    LLBC_Defer(LLBC_Cleanup());

    // ����һ��������
    LLBC_Stream stream;
    // ͨ��ToString()������Ĭ��Stream��Ϣ
    LLBC_PrintLn("Default stream: %s", stream.ToString().c_str());

    // �����ֽ���Ϊnetworld byte order, Ĭ��ΪLLBC_DefaultEndian, ��ͨ��Config.h�е������޸�Ĭ���ֽ���
    stream.SetEndian(LLBC_Endian::NetEndian);

    // ========================= ���������������л�, �����л� =========================
    LLBC_PrintLn("Basic datatypes serialize/deserialize:");
    // ���л�numbers/string����
    stream << true // bool
           << -32 // int32
           << 32u // uint32
           << -64ll // int64
           << 64llu // uint64
           << 3.14f // float
           << 6.28 // double
           << "Hello World!"; // string
    // ��ӡstream��Ϣ
    LLBC_PrintLn("- After serialize numbers/string, stream: %s", stream.ToString().c_str());

    // ����������stream(attach from buffer or another stream).
    LLBC_Stream stream2;
    stream2.Attach(stream);
    LLBC_PrintLn("- Attach from stream, stream2: %s", stream2.ToString().c_str());

    // �����л�numbers/string����
    bool boolVal;
    sint32 int32Val;
    uint32 uint32Val;
    sint64 int64Val;
    uint64 uint64Val;
    float floatVal;
    double doubleVal;
    LLBC_String strVal;
    stream2 >> boolVal
            >> int32Val
            >> uint32Val
            >> int64Val
            >> uint64Val
            >> floatVal
            >> doubleVal
            >> strVal;
    // ��ӡstream��Ϣ�������л���ĸ�numbers/stringֵ(ʹ��streaming��ʽ���)
    std::cout << "- After deserialize numbers/string, stream: " << stream << std::endl;
    LLBC_PrintLn("  - boolVal: %d", boolVal);
    LLBC_PrintLn("  - int32Val: %d", int32Val);
    LLBC_PrintLn("  - uint32Val: %d", uint32Val);
    LLBC_PrintLn("  - int64Val: %lld", int64Val);
    LLBC_PrintLn("  - uint64Val: %llu", uint64Val);
    LLBC_PrintLn("  - floatVal: %f", floatVal);
    LLBC_PrintLn("  - doubleVal: %f", doubleVal);
    LLBC_PrintLn("  - strVal: %s", strVal.c_str());
    LLBC_PrintLn("\n");

    // ========================= stl�������л�/�����л� =========================
    LLBC_PrintLn("Stl container serialize/deserialize:");
    std::vector<std::map<LLBC_String, std::set<int> > > stlContainer = {
        {
            { "Key1", {1, 2, 3} },
            { "Key2", {4, 5, 6} }
        }
    };

    // reset stream��ָ��
    // stream.SetReadPos(0); // ����reset readָ��, SetWritePos(0)ʱ, readָ�뽫�Զ�����
    // reset streamдָ��
    stream.SetWritePos(0);

    // ִ�ж�д
    decltype(stlContainer) stlContainer2;
    stream << stlContainer >> stlContainer2;
    // Dump���·����л������Ľ��(ͨ��LLBC_Variant��ӡ��������)
    LLBC_PrintLn("- After write & read stl container: %s, container:", LLBC_GetTypeName(stlContainer));
    LLBC_PrintLn("  - %s", LLBC_Variant(stlContainer2).ValueToString().c_str());
    LLBC_PrintLn("\n");

    // ========================= �û��Զ��������л� =========================
    LLBC_PrintLn("User define class/struct serialize/deserialize:");
    PlayerData playerData {9527llu, "Judy", 99, PlayerSex::Female};

    stream.SetWritePos(0);
    PlayerData playerData2;
    stream << playerData >> playerData2;
    LLBC_PrintLn("- After write & read player data, player data:");
    LLBC_PrintLn("  - %s", playerData2.ToString().c_str());

    return LLBC_OK;
}
