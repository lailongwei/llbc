/**
 * @file    TestCase_Core_File_File.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/07
 * @version 1.0
 *
 * @brief
 */

#include "core/file/TestCase_Core_File_File.h"

int TestCase_Core_File_File::Run(int argc, char *argv[])
{
    LLBC_PrintLine("core/file test:");
    
    LLBC_PrintLine("Current working directory: %s", LLBC_Directory::GetCurrentDirectory().c_str());
    
    LLBC_String tempPath = LLBC_GetTemporaryDirectory(true);
    this->OpenCloseTest(tempPath + "a.txt"), LLBC_PrintLine("");
    this->OpenCloseTest(tempPath + "."), LLBC_PrintLine("");

    this->SetBufferModeTest(tempPath + "a.txt"), LLBC_PrintLine("");
    try
    {
        this->SetBufferModeTest(tempPath + ".");
    }
    catch (const LLBC_IOException &e)
    {
        LLBC_PrintLine("ioexception generation: %s", e.what() );
    }
    LLBC_PrintLine("");

    this->RWTest(tempPath + "a.txt");
    LLBC_PrintLine("");

    this->DeleteTest(tempPath + "a.txt");
    LLBC_PrintLine("");

    this->MoveTest(tempPath + "a.txt", tempPath + "b.txt");
    LLBC_PrintLine("");

    this->CopyTest(tempPath + "a.txt", tempPath + "b.txt");
    LLBC_PrintLine("");

    LLBC_PrintLine("Press any key to continue ...");
    getchar();

    return 0;
}

void TestCase_Core_File_File::OpenCloseTest(const LLBC_String &file)
{
    LLBC_PrintLine("Open()/Close() APIs test:");

    // open file.
    LLBC_PrintLine("open file: %s", file.c_str() );
    if(m_file1.Open(file, "wb") != LLBC_RTN_OK)
    {
        LLBC_PrintLine("open file[%s] failed: %s", 
            file.c_str(), LLBC_FormatLastError() );
        return;
    }

    // check opened or not.
    LLBC_PrintLine("file[%s] is opened? %s", file.c_str(), m_file1.IsOpened() ? "true" : "false");
    LLBC_PrintLine("file[%s] exist? %s", file.c_str(), LLBC_File::Exist(file) ? "true" : "false");

    // close it.
    LLBC_PrintLine("close file: %s", file.c_str() );
    if(m_file1.Close() != LLBC_RTN_OK)
    {
        LLBC_PrintLine("close file[%s] failed: %s", 
            file.c_str(), LLBC_FormatLastError() );
        return;
    }

    // reopen it.
    LLBC_PrintLine("reopen file: %s", file.c_str() );
    if(m_file1.Open(file) != LLBC_RTN_OK)
    {
        LLBC_PrintLine("reopen file[%s] failed, reason: %s",
            file.c_str(), LLBC_FormatLastError() );
        return;
    }

    LLBC_PrintLine("close file %s", file.c_str() );
    if(m_file1.Close() != LLBC_RTN_OK)
    {
        LLBC_PrintLine("close file[%s] failed, reason: %s",
            file.c_str(), LLBC_FormatLastError() );
        return;
    }
}

void TestCase_Core_File_File::SetBufferModeTest(const LLBC_NAMESPACE LLBC_String &file)
{
    LLBC_PrintLine("SetBufferMode() test:");
    LLBC_PrintLine("open file(use exception throw version API) %s:", file.c_str());

    m_file1.OpenT(file);

    LLBC_PrintLine("set buffer mode: FULL, size: 128");
    if(m_file1.SetBufferMode(LLBC_FileBufferMode::FullBuf, 128) != LLBC_RTN_OK)
    {
        LLBC_PrintLine("set buffer mode[FULL] failed, reason: %s", LLBC_FormatLastError());
        return;
    }

    LLBC_PrintLine("set buffer mode: LINE, size:128");
    if(m_file1.SetBufferMode(LLBC_FileBufferMode::LineBuf, 128) != LLBC_RTN_OK)
    {
        LLBC_PrintLine("set buffer mode[LINE] failed, reason: %s", LLBC_FormatLastError());
        return;
    }

    LLBC_PrintLine("set buffer mode: NO_BUF, size: 0");
    if(m_file1.SetBufferMode(LLBC_FileBufferMode::NoBuf, 0) != LLBC_RTN_OK)
    {
        LLBC_PrintLine("set buffer mode[NO_BUF] failed, reason: %s", LLBC_FormatLastError());
        return;
    }

    LLBC_PrintLine("close file.");
    m_file1.Close();

    LLBC_PrintLine("try to set closed file's buffer mode:");
    if(m_file1.SetBufferMode(LLBC_FileBufferMode::FullBuf, 128) != LLBC_RTN_OK)
        LLBC_PrintLine("set buffer mode failed, right!!!!, reason: %s", LLBC_FormatLastError());
    else
        LLBC_PrintLine("set buffer mode successed, bad!!!");

    return;
}

void TestCase_Core_File_File::RWTest(const LLBC_NAMESPACE LLBC_String &file)
{
    LLBC_PrintLine("Read/Write APIs test:");

    // open file.
    LLBC_PrintLine("Open file: %s", file.c_str());
    m_file1.OpenT(file, "wb+");

    // serialize.
    LLBC_PrintLine("write basic datas(Use writeObjT() to write)");
    m_file1.WriteObjT((sint8)-8 ), m_file1.WriteObjT( (uint8)8);
    m_file1.WriteObjT((sint16)-16 ), m_file1.WriteObjT( (uint16)16);
    m_file1.WriteObjT((sint32)-32 ), m_file1.WriteObjT( (uint32)32);
    m_file1.WriteObjT((sint64)-64 ), m_file1.WriteObjT( (uint64)64);
    m_file1.WriteObjT((float)0.1 ), m_file1.WriteObjT( (double)0.2);

    LLBC_PrintLine("write basic datas(Use operator << to write)");
    m_file1 <<(sint8)-8 <<(uint8)8;
    m_file1 <<(sint16)-16 <<(uint16)16;
    m_file1 <<(sint32)-32 <<(uint32)32;
    m_file1 <<(sint64)-64 <<(uint64)64;
    m_file1 <<(float)0.1 <<(double)0.2;

    LLBC_String strData = "String Data(Use WriteStringT() write)!";
    LLBC_PrintLine("Write string data: %s", strData.c_str());
    m_file1.WriteStringT(strData);

    strData = "String Data(Use operator << write)!";
    LLBC_PrintLine("Write string data: %s", strData.c_str());
    m_file1 <<strData;

    strData = "line data!!!!";
    LLBC_PrintLine("Write line data: %s", strData.c_str());
    m_file1.WriteLineT(strData);

    LLBC_Stream stream;
    stream.WriteBuffer("Stream data", 12);
    LLBC_PrintLine("write stream data: %s", 
        reinterpret_cast<const char *>( stream.GetBuf() ));
    m_file1 <<stream;

    // flush it.
    LLBC_PrintLine("flush data");
    m_file1.FlushT();

    // seek to begin.
    LLBC_PrintLine("seek to begin");
    m_file1.SeekT(LLBC_FilePos::Begin, 0);

    // deserialize.
    sint8 sint8Val; uint8 uint8Val;
    sint16 sint16Val; uint16 uint16Val;
    sint32 sint32Val; uint32 uint32Val;
    sint64 sint64Val; uint64 uint64Val;
    float floatVal; double doubleVal;

    LLBC_PrintLine("get basic data(use ReadObjT() to read):");
    m_file1.ReadObjT(sint8Val); m_file1.ReadObjT(uint8Val);
    m_file1.ReadObjT(sint16Val); m_file1.ReadObjT(uint16Val);
    m_file1.ReadObjT(sint32Val); m_file1.ReadObjT(uint32Val);
    m_file1.ReadObjT(sint64Val); m_file1.ReadObjT(uint64Val);
    m_file1.ReadObjT(floatVal); m_file1.ReadObjT(doubleVal);
    std::cout <<"sint8Val: " <<sint8Val <<", uint8Val: " <<uint8Val <<"\n";
    std::cout <<"sint16Val: " <<sint16Val <<", uint16Val: " <<uint16Val <<"\n";
    std::cout <<"sint32Val: " <<sint32Val <<", uint32Val: " <<uint32Val <<"\n";
    std::cout <<"sint64Val: " <<sint64Val <<", uint64Val: " <<uint64Val <<"\n";
    std::cout <<"floatVal: " <<floatVal <<", doubleVal: " <<doubleVal <<"\n";

    LLBC_PrintLine("get basic data(use operator >>() to read):");
    m_file1 >>sint8Val >>uint8Val;
    m_file1 >>sint16Val >>uint16Val;
    m_file1 >>sint32Val >>uint32Val;
    m_file1 >>sint64Val >>uint64Val;
    m_file1 >>floatVal >>doubleVal;
    std::cout <<"sint8Val: " <<sint8Val <<", uint8Val: " <<uint8Val <<"\n";
    std::cout <<"sint16Val: " <<sint16Val <<", uint16Val: " <<uint16Val <<"\n";
    std::cout <<"sint32Val: " <<sint32Val <<", uint32Val: " <<uint32Val <<"\n";
    std::cout <<"sint64Val: " <<sint64Val <<", uint64Val: " <<uint64Val <<"\n";
    std::cout <<"floatVal: " <<floatVal <<", doubleVal: " <<doubleVal <<"\n";

    LLBC_PrintLine("read string data(use ReadStringT() to read):");
    strData.clear();
    m_file1.ReadStringT(strData);
    std::cout <<"string data: " <<strData.c_str() <<std::endl;

    LLBC_PrintLine("read string data(use operator >>() to read):");
    strData.clear();
    m_file1 >>strData;
    std::cout <<"string data: " <<strData.c_str() <<std::endl;

    LLBC_PrintLine("read line data:");
    strData.clear();
    m_file1.ReadLineT(strData);
    std::cout <<"line data: " <<strData.c_str() <<std::endl;

    LLBC_PrintLine("read stream data:");
    stream.SetPos(0);
    m_file1 >>stream;
    std::cout <<"stream data: " <<(const char *)stream.GetBuf() <<std::endl;

    LLBC_PrintLine("now file pos: %ld, size: %ld",
        m_file1.TellT(), m_file1.GetSizeT() );

    LLBC_PrintLine("close file");
    m_file1.CloseT();
}

void TestCase_Core_File_File::DeleteTest(const LLBC_NAMESPACE LLBC_String &file)
{
    LLBC_PrintLine("Delete file test:");

    LLBC_PrintLine("open file: %s", file.c_str());
    m_file1.OpenT(file, "wb+");

    LLBC_PrintLine("delete it");
    m_file1 <<"asfasflkas;lfas";
    m_file1.DeleteSelfT();

    LLBC_PrintLine("now exist? %s", LLBC_File::Exist(file) ? "true" : "false");
}

void TestCase_Core_File_File::MoveTest(const LLBC_String &file1,
                                       const LLBC_String &file2)
{
    std::cout <<"Move file test:" <<std::endl;

    std::cout <<"Open file " <<file1 <<":" <<std::endl;
    m_file1.OpenT(file1, "wb");

    LLBC_String strData = "This is a string data!";
    std::cout <<"Write string: " <<strData <<std::endl;
    m_file1 <<strData;

    std::cout <<"Move file [" <<file1 <<"] -> [" <<file2 <<"]:" <<std::endl;
    m_file1.MoveSelfT(file2, true, true, "rb");

    std::cout <<"Read data from the moved file" <<std::endl;
    strData.clear();
    m_file1 >>strData;

    std::cout <<"Data is: " <<strData <<std::endl;

    m_file1.CloseT();
    LLBC_File::DeleteT(file2);
}

void TestCase_Core_File_File::CopyTest(const LLBC_String &file1,
                                       const LLBC_String &file2)
{
    std::cout <<"Copy file test:" <<std::endl;
    
    std::cout <<"Open file [" <<file1 <<"]:" <<std::endl;
    m_file1.OpenT(file1, "wb");

    LLBC_String strData = "String Data!";
    std::cout <<"Write data: " <<strData <<std::endl;
    m_file1 <<strData;

    std::cout <<"Copy to [" <<file2 <<"]" <<std::endl;
    m_file1.CopySelfT(file2, true, NULL, 1024);

    std::cout <<"Open copy file." <<std::endl;
    m_file2.OpenT(file2);

    strData.clear();
    m_file2 >>strData;
    std::cout <<"Read data from copy file: " <<strData <<std::endl;

    m_file1.DeleteSelfT();
    m_file2.DeleteSelfT();
}
