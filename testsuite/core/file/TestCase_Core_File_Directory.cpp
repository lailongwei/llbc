/**
 * @file    TestCase_Core_File_Directory.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/04/09
 * @version 1.0
 *
 * @brief
 */

#include "core/file/TestCase_Core_File_Directory.h"

int TestCase_Core_File_Directory::Run(int argc, char *argv[])
{
    std::cout <<"core/file/Directory test:" <<std::endl;

    const LLBC_String tempPath = LLBC_GetTemporaryDirectory(true);
    
    LLBC_String str = tempPath + "a";
    std::cout <<"Create directory: " <<str <<std::endl;

    if(LLBC_Directory::DeleteDir(tempPath + "a") != LLBC_RTN_OK)
    {
        std::cout <<"delete directory 'a' failed, reason: " <<LLBC_FormatLastError() <<std::endl;
    }
    
    LLBC_Directory dir;
    dir.SetDirT(str);
    dir.CreateDirT(str);

    std::cout <<"Create sub dir: b,c,d,e,f" <<std::endl;
    dir.CreateSubDirT("b");
    dir.CreateSubDirT("c");
    dir.CreateSubDirT("d");
    dir.CreateSubDirT("e");
    dir.CreateSubDirT("f");

    std::cout <<"Set directory a/b" <<std::endl;
    dir.SetDirT(tempPath + "a/b");
    std::cout <<"a/b exist? " <<(dir.Exist() ? "true" : "false") <<std::endl;

    std::cout <<"Create sub dir: x,y,z" <<std::endl;
    dir.CreateSubDirT("x");
    dir.CreateSubDirT("y");
    dir.CreateSubDirT("z");

    std::cout <<"Create a/1.txt, a/b/2.txt, a/b/x/3.txt, a/b/x/4.txt" <<std::endl;
    LLBC_File::TouchT(tempPath + "a/1.txt");
    LLBC_File::TouchT(tempPath + "a/b/2.txt");
    LLBC_File::TouchT(tempPath + "a/b/x/3.txt");
    LLBC_File::TouchT(tempPath + "a/b/x/4.txt");

    std::cout <<"Obtain files test:" <<std::endl;
    std::vector<LLBC_String> files;
    dir.SetDir(tempPath + "a");
    dir.GetFilesT(files, "", true);
    std::cout <<"file count: " <<files.size() <<std::endl;
    for(size_t i = 0; i < files.size(); i ++)
    {
        std::cout <<"    " <<files[i] <<std::endl;
    }

    std::cout <<"Obtain directories test:" <<std::endl;
    std::vector<LLBC_String> directories;
    dir.GetDirectoriesT(directories, "", true);
    std::cout <<"directories count: " <<directories.size() <<std::endl;
    for(size_t i = 0; i < directories.size(); i ++)
    {
        std::cout <<"    " <<directories[i] <<std::endl;
    }

    std::cout <<"delete all test directories and files" <<std::endl;
    dir.DeleteSelfDirT();

    std::cout <<"Press any key to continue ..." <<std::endl;
    getchar();

    return 0;
}
