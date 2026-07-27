---
layout: default
title: 委托 Delegate
---

# 委托 Delegate

`LLBC_Delegate` 是通用委托封装类，用于实现类型安全的回调机制。它是一个轻量级的函数包装器，
可将各种可调用对象（普通函数指针、成员函数、Lambda、`std::function`）统一封装为同一种委托对象，
提供类似 C# 委托的直观语法。

模板以**函数签名**作为参数：`LLBC_Delegate<ReturnType(ParamTypes...)>`。

## 核心特性

- **类型安全**：编译期强类型检查，避免类型错误。
- **零开销直调**：C 函数指针走直接调用路径，无虚函数、无 `std::function` 包装开销。
- **多种可调用对象**：普通函数、成员函数、Lambda 表达式、`std::function`。
- **完整值语义**：支持拷贝、移动与赋值。
- **空值语义**：可赋值为 `nullptr`，支持空委托检查。

## 绑定普通函数

构造时绑定，或先声明再赋值绑定，两种方式等价：

```cpp
void PrintMessage(const std::string &msg)
{
    std::cout << "Message: " << msg << std::endl;
}

// 方式 1：构造时绑定
LLBC_Delegate<void(const std::string &)> deleg1(&PrintMessage);
deleg1("Hello from delegate");

// 方式 2：赋值绑定
LLBC_Delegate<void(const std::string &)> deleg2;
deleg2 = &PrintMessage;
deleg2("Hello again");
```

## 绑定成员函数

通过 `(对象指针, &类::成员方法)` 两参数构造，省去手动 `std::bind`。普通成员函数与 `const` 成员函数均支持：

```cpp
class GameLogic
{
public:
    void OnTimeout(LLBC_Timer *timer)
    {
        std::cout << "Timer fired, ID: " << timer->GetTimerId() << std::endl;
    }
};

GameLogic logic;
LLBC_Timer timer(LLBC_Delegate<void(LLBC_Timer *)>(&logic, &GameLogic::OnTimeout));
```

## 绑定 Lambda 表达式

Lambda（以及其它任意可调用对象、`std::function`）可直接赋值或构造：

```cpp
LLBC_Delegate<void(int)> deleg = [](int value) {
    std::cout << "Lambda called with value: " << value << std::endl;
};
deleg(42);
```

## 空委托检查与重置

委托支持 `explicit operator bool` 与 `nullptr` 比较，调用前应先判空：

```cpp
void SafeCallback(LLBC_Delegate<void()> &callback)
{
    // 方式 1：bool 转换检查
    if (callback)
        callback();

    // 方式 2：与 nullptr 比较
    if (callback != nullptr)
        callback();

    // 方式 3：重置为空
    callback = nullptr;
}
```

<div class="note" markdown="1">
`operator bool` 被声明为 `explicit`，因此可用于 `if (callback)` 这类布尔上下文，
但不会隐式转换为整型等类型。
</div>

## 设计与实现

`LLBC_Delegate` 采用**双路存储策略**，任意时刻只有一路有效（由构造/赋值时的初始化控制）：

```
+---------------------+
|  LLBC_Delegate      |
|---------------------|
| - CFunc   _cfunc    |  ← 策略 1：C 函数指针，直接调用
| - StlFunc _stlFunc  |  ← 策略 2：std::function
|---------------------|
| + operator()(...)   |  ← 统一接口
+---------------------+
```

- **CFunc**（`Rtn(*)(Args...)`）：存储裸 C 函数指针，`operator()` 直接调用，无额外开销。
- **StlFunc**（`std::function<Rtn(Args...)>`）：承载成员函数、Lambda、`std::bind` 等复杂可调用对象；
  成员函数经内部 `LLBC_BindClassMethod(obj, meth)` 绑定后存入。

调用时优先走 `_cfunc`：命中则直调，否则回退到 `_stlFunc`，从而在保留通用性的同时，
为 C 函数指针保留零开销路径。

在模板层面，通过**先声明主模板、再偏特化定义**的方式，将 `LLBC_Delegate` 严格限制为
函数签名式使用 `LLBC_Delegate<Rtn(Args...)>`：利用 C++ 函数类型作为模板参数，
统一承载返回值、参数列表与调用语义。

## 参照

- 头文件：`llbc/include/llbc/core/utils/Util_Delegate.h`（内联实现见同目录 `Util_DelegateInl.h`）
- 示例：`tests/func_test/core/utils/FuncTest_Core_Utils_Delegate.cpp`
- 快速上手示例（可跑）：`tests/example/core/Example_Core_Delegate.cpp`
