---
layout: default
title: 配置 Ini / Properties / Json / Xml
---

# 配置 Ini / Properties / Json / Xml

llbc 内置两种文本配置格式的原生支持：传统 INI（`LLBC_Ini`）和 Java 风格 `.properties`（`LLBC_Properties`）。
JSON 与 XML 以第三方库形式捆绑随库发布——rapidjson 负责 JSON、tinyxml2 负责 XML——
可直接包含其头文件使用，无需额外链接步骤。

所有配置 API 均属于 `core` 模块，随 `#include "llbc.h"` 或 `#include "llbc/core/Core.h"` 引入。

## LLBC_Ini — 加载与读取

`LLBC_Ini` 支持从文件或字符串加载 INI，返回 `LLBC_OK` / `LLBC_FAILED`；失败时通过
`GetLoadError()` 获取精确错误行列信息，再通过 `LLBC_GetLastError()` 获取错误码。

```cpp
LLBC_Ini ini;
if (ini.LoadFromFile("server.ini") != LLBC_OK)
{
    LLBC_PrintLn("load error: %s", ini.GetLoadError().c_str());
    return LLBC_FAILED;
}

// 按 section + key 读取，泛型，不存在时返回默认值
int   port    = ini.GetValue<int>("Network", "port", 7788);
float timeout = ini.GetValue<float>("Network", "timeout", 30.0f);
LLBC_String host = ini.GetValue<LLBC_String>("Network", "host", "127.0.0.1");
```

支持的类型：`bool`、`sint8/uint8`、`sint16/uint16`、`sint32/uint32`、
`long/ulong`、`sint64/uint64`、`float`、`double`、`LLBC_String`、`std::string`。

## LLBC_Ini — Section 对象

当需要读取同一 section 下多个键时，直接拿 `LLBC_IniSection *` 指针效率更高。

```cpp
const LLBC_IniSection *net = ini.GetSection("Network");
if (net && net->IsHasKey("port"))
{
    int port = net->GetValue<int>("port", 0);
    LLBC_String comment = net->GetComment("port"); // 读取注释
}

// 遍历所有 section
for (auto &kv : ini.GetAllSections())
    LLBC_PrintLn("section: %s", kv.first.c_str());
```

## LLBC_Ini — 修改与保存

`LLBC_IniSection` 可独立构建后再注入 `LLBC_Ini`，支持 `merge` 合并模式。

```cpp
LLBC_IniSection sec;
sec.SetValue("maxConn", 1000, "maximum connections");
sec.SetValue("debug", false);
ini.SetSection("Network", sec, /*merge=*/true);

// 保存（可选按 section/key 排序）
ini.SaveToFile("server_out.ini", {}, /*sortSections=*/true, /*sortKeys=*/false);
```

<div class="callout note" markdown="1">
**错误处理**：`GetValue<T>` 在 section 存在但 key 不存在时返回 `defaultValue` 并设置
`LLBC_ERROR_NOT_FOUND`（section 本身不存在时直接返回 `defaultValue`），不会抛出异常。
在需要区分"不存在"和"值恰为默认"的场景，先调用 `IsHasKey()` 或检查 `LLBC_GetLastError()`。
</div>

## LLBC_Properties — 层级属性

`LLBC_Properties` 是纯静态工具类，将 `.properties` 文件解析为嵌套 `LLBC_Variant` 树；
点分 key（`llbc.server.port`）映射为多层 `operator[]` 访问。

```cpp
LLBC_Variant props;
LLBC_String  errMsg;

// 从文件加载
if (LLBC_Properties::LoadFromFile("app.properties", props, &errMsg) != LLBC_OK)
{
    LLBC_PrintLn("load failed: %s", errMsg.c_str());
    return LLBC_FAILED;
}

// 按层级访问
LLBC_String host = props["llbc"]["server"]["host"].ValueToString();
int         port = props["llbc"]["server"]["port"].As<int>();
```

## LLBC_Properties — 构造与序列化

```cpp
LLBC_Variant cfg;
cfg["llbc"]["server"]["host"] = "0.0.0.0";
cfg["llbc"]["server"]["port"] = 7788;
cfg["llbc"]["server"]["debug"] = false;

LLBC_String content;
LLBC_String errMsg;
LLBC_Properties::SaveToString(cfg, content, &errMsg);
// 写回文件
LLBC_Properties::SaveToFile(cfg, "app_out.properties", &errMsg);
```

<div class="callout warning" markdown="1">
**键名约束**：Properties key 中每一段不得包含空白字符或 `#`（注释起始符），
违规时 `SaveToString` / `SaveToFile` 返回 `LLBC_FAILED` 并填充 `errMsg`。
</div>

## JSON — 内置 rapidjson

llbc 捆绑了 rapidjson，头文件位于 `llbc/include/llbc/core/rapidjson/`。
直接 `#include` 其标准入口即可，无需修改链接配置：

```cpp
#include "llbc/core/rapidjson/document.h"
#include "llbc/core/rapidjson/writer.h"
#include "llbc/core/rapidjson/stringbuffer.h"

using namespace rapidjson;

Document doc;
doc.Parse(R"({"port":7788,"host":"127.0.0.1"})");
if (!doc.HasParseError())
{
    int port = doc["port"].GetInt();
    const char *host = doc["host"].GetString();
}
```

rapidjson 本身是纯头文件库，API 与上游完全一致，详见 rapidjson 官方文档。

## XML — 内置 tinyxml2

XML 解析由捆绑的 tinyxml2 提供，入口头文件为
`llbc/include/llbc/core/tinyxml2/tinyxml2.h`：

```cpp
#include "llbc/core/tinyxml2/tinyxml2.h"

tinyxml2::XMLDocument xmlDoc;
if (xmlDoc.LoadFile("config.xml") == tinyxml2::XML_SUCCESS)
{
    auto *root = xmlDoc.FirstChildElement("Config");
    auto *net  = root ? root->FirstChildElement("Network") : nullptr;
    int   port = net  ? net->IntAttribute("port", 7788) : 7788;
}
```

tinyxml2 同样是纯头文件 + 单编译单元库，API 与上游完全一致。

<div class="callout important" markdown="1">
**线程安全**：`LLBC_Ini` 与 `LLBC_Properties` 均 **非线程安全**。多线程场景下请在
外部加锁，或在初始化阶段完成加载后以只读方式共享。
</div>

## 参照

- 头文件：`llbc/include/llbc/core/config/Ini.h`
- 头文件：`llbc/include/llbc/core/config/IniInl.h`
- 头文件：`llbc/include/llbc/core/config/Properties.h`
- 真实示例（func_test）：`tests/func_test/core/config/FuncTest_Core_Config_Ini.cpp`
- 真实示例（func_test）：`tests/func_test/core/config/FuncTest_Core_Config_Properties.cpp`

## 下一步

- [Variant 通用值容器](variant.md) — Properties 解析结果的底层类型
- [日志](logger.md) — 配置文件错误通常通过日志输出
