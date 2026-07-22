---
layout: default
title: 文件与目录
---

# 文件与目录

`LLBC_File` 和 `LLBC_Directory` 是 llbc `core` 模块提供的跨平台文件系统工具。
`LLBC_File` 封装单文件的打开、读写、定位与属性操作；`LLBC_Directory` 以纯静态方法
提供目录的存在检测、创建/删除、遍历以及路径拼接，覆盖 Linux / macOS / Windows 三平台。

## 打开与关闭文件

`LLBC_File` 可在构造时直接打开，也可先默认构造再调用 `Open()`。
打开是否成功通过 `IsOpened()` 或返回值判断，失败细节用 `LLBC_FormatLastError()` 获取。

```cpp
// 构造时打开（BinaryRead 为默认模式，文件必须存在）
LLBC_File rf("data.bin", LLBC_FileMode::BinaryRead);
if (!rf.IsOpened())
{
    LLBC_PrintLn("open failed: %s", LLBC_FormatLastError());
    return LLBC_FAILED;
}

// 先构造再打开（BinaryWrite：不存在则创建，存在则截断）
LLBC_File wf;
if (wf.Open("out.bin", LLBC_FileMode::BinaryWrite) != LLBC_OK)
    return LLBC_FAILED;

// 以最近一次模式重新打开（常用于截断重写场景）
wf.ReOpen(LLBC_FileMode::BinaryReadWrite);
wf.Close();
```

<div class="callout note" markdown="1">
`LLBC_File` 禁止拷贝赋值（`LLBC_DISABLE_ASSIGNMENT`）。若需在容器中使用，请用指针或 `std::unique_ptr`。
</div>

## 文件打开模式

| 常量 | 说明 |
|------|------|
| `BinaryRead` | 二进制只读，文件必须存在（默认） |
| `BinaryWrite` | 二进制写入，不存在则创建，存在则截断 |
| `BinaryReadWrite` | 二进制读写，行为同 Write |
| `BinaryAppendWrite` | 二进制追加写 |
| `BinaryAppendReadWrite` | 二进制追加读写 |
| `Text*` 系列 | 以上各模式的文本格式对应版本（`TextRead` 等） |
| `LastestMode` | 供 `ReOpen()` 使用，沿用上次模式 |

## 读写文件

`Write` / `Read` 提供基础类型重载（`bool`、`sint8`…`double` 等），也支持直接读写字节块、
字符串行及格式化写入。所有写操作都可能返回 `LLBC_FAILED`（截断时 `LLBC_GetLastError()` 为
`LLBC_ERROR_TRUNCATED`）。

```cpp
LLBC_File f("record.bin", LLBC_FileMode::BinaryReadWrite);

// 写入多种类型
f.Write(static_cast<sint32>(42));
f.Write(static_cast<double>(3.14));
f.WriteLn("Hello World!");          // 写一行（自动匹配换行符）

// 格式化写入
f.FormatWrite("level=%d score=%.2f name=%s", 10, 99.5, "Judy");

// 回到文件头，读取
f.Seek(LLBC_FileSeekOrigin::Begin, 0);
sint32 level; double score;
f.Read(level);
f.Read(score);
LLBC_String line = f.ReadLn();      // 读一行（不含换行符）

f.Flush();
```

一次性读全文（静态便捷接口，无需手动打开）：

```cpp
LLBC_String content = LLBC_File::ReadToEnd("config.txt");
if (LLBC_GetLastError() != LLBC_ERROR_SUCCESS)
    LLBC_PrintLn("read failed: %s", LLBC_FormatLastError());
```

## 文件定位与大小

```cpp
LLBC_File f("data.bin", LLBC_FileMode::BinaryReadWrite);
sint64 size = f.GetFileSize();

// Seek 三种原点：Begin / Current / End
f.Seek(LLBC_FileSeekOrigin::End, -8);   // 跳到最后 8 字节
sint64 pos = f.GetFilePosition();

f.SetFilePosition(0);                    // 等价于 Seek(Begin, 0)
sint64 readable = f.GetReadableSize();   // 当前位置到末尾的可读字节数

f.Truncate(0);                           // 清空文件内容
```

## 文件属性与静态工具

```cpp
// 检查文件是否存在
if (!LLBC_File::Exists("config.ini"))
    LLBC_PrintLn("not found");

// 获取属性（大小、可读写、时间戳等）
LLBC_FileAttributes attrs;
if (LLBC_File::GetFileAttributes("config.ini", attrs) == LLBC_OK)
    LLBC_PrintLn("size=%lld writable=%d", attrs.fileSize, attrs.writable);

// 复制 / 移动 / 删除
LLBC_File::CopyFile("src.bin", "dst.bin", /*overlapped=*/true);
LLBC_File::MoveFile("old.bin", "new.bin");
LLBC_File::DeleteFile("tmp.bin");
```

<div class="callout warning" markdown="1">
`CopyFile` / `MoveFile` 的第三个参数 `overlapped` 默认为 `false`（目标已存在时失败）。
需要覆盖时必须显式传 `true`。
</div>

## 目录存在检测与创建

`LLBC_Directory` 的所有方法都是静态的，不需要实例。`Create` / `Remove` 均为**递归**操作。

```cpp
// 检查目录是否存在
if (!LLBC_Directory::Exists("logs/2026"))
{
    // 递归创建多级目录
    if (LLBC_Directory::Create("logs/2026/07") != LLBC_OK)
        LLBC_PrintLn("mkdir failed: %s", LLBC_FormatLastError());
}

// 递归删除整棵目录树
LLBC_Directory::Remove("logs/2026");
```

## 遍历文件与子目录

```cpp
LLBC_Strings files, dirs;

// 列出当前目录下所有 .cpp 文件（非递归）
LLBC_Directory::GetFiles("src", files, ".cpp", /*recursive=*/false);

// 递归列出所有文件
LLBC_Directory::GetFiles("src", files, /*recursive=*/true);

// 列出子目录（递归）
LLBC_Directory::GetDirectories("src", dirs, /*recursive=*/true);

for (const auto &f : files)
    LLBC_PrintLn("file: %s", f.c_str());
```

## 路径拼接与解析

`Join` 支持 2 个、多个变参、以及 `LLBC_Strings` 列表三种重载；
跨平台处理分隔符。

```cpp
// 2 段路径拼接
auto cfgPath = LLBC_Directory::Join(LLBC_Directory::ModuleFileDir(), "config.ini");

// 多段变参（任意个数）
auto logPath = LLBC_Directory::Join(LLBC_Directory::HomeDir(), "logs", "2026", "app.log");

// 拆分扩展名：parts[0] = 无扩展名路径, parts[1] = ".exe"
LLBC_Strings parts = LLBC_Directory::SplitExt("a/b/foo.exe");

// DirName / BaseName
LLBC_String dir  = LLBC_Directory::DirName("/usr/bin/app");   // "/usr/bin"
LLBC_String base = LLBC_Directory::BaseName("/usr/bin/app");  // "app"

// 转为规范化绝对路径
LLBC_String abs = LLBC_Directory::AbsPath("../conf");
```

## 特殊目录

```cpp
LLBC_Directory::ModuleFileDir();   // 可执行文件所在目录
LLBC_Directory::ModuleFilePath();  // 可执行文件完整路径
LLBC_Directory::CurDir();         // 当前工作目录
LLBC_Directory::TempDir();        // 临时目录
LLBC_Directory::HomeDir();        // 用户主目录
LLBC_Directory::CacheDir();       // 缓存目录
```

<div class="callout note" markdown="1">
`SetCurDir()` 会改变进程级工作目录，多线程环境下需格外谨慎。
</div>

## 参照

- 头文件：`llbc/include/llbc/core/file/File.h`、`FileInl.h`
- 头文件：`llbc/include/llbc/core/file/Directory.h`、`DirectoryInl.h`
- 功能测试：`tests/func_test/core/file/FuncTest_Core_File_File.cpp`
- 功能测试：`tests/func_test/core/file/FuncTest_Core_File_Directory.cpp`

## 下一步

- [序列化 Stream](stream.md) — 读写二进制数据流
- [Variant 通用值](variant.md) — 动态类型容器
