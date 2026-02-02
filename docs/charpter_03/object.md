# 对象 - LLBC_Object

## 概述

LLBC_Object 是所有可被引用管理或自动释放的对象基类. 提供非侵入性的对象生命周期管理机制, 并支持线程安全的引用计数操作.

## 使用示例

1. 手动 Retain / Release(显示生命周期管理):

```cpp
class MyClass : public LLBC_Object
{
    // ...
};

MyClass* obj = new MyClass();
obj->Retain(); // 增加引用计数
obj->Release(); // 减少引用计数
obj->Release(); // 最后一次 Release 会删除对象
```

2. AutoRelease(自动生命周期管理):

```cpp
class MyClass : public LLBC_Object
{
    // ...
};

MyClass* obj = new MyClass();
obj->Retain(); // 增加引用计数
LLBC_AutoReleasePool::Current()->Add(obj); // 委托给自动释放池管理, 在 servivce 帧尾或析构时计数减 1
```

## 设计与实现

LLBC_Object 的设计目标可以归纳为四点：
1.	统一生命周期管理模型
2.	支持自动释放池
3.	可扩展、可继承
4.	跨线程 / 跨模块安全

核心成员:
```cpp
int _ref;        // 引用计数
int _autoRef;   // 自动释放引用计数
LLBC_AutoReleasePoolStack *_poolStack;
```

_ref: 用于手动 Retain / Release 的引用计数.
_autoRef: 用于 AutoRelease 池管理的引用计数.
_poolStack: 指向当前对象所属的自动释放池栈.

#### Retain / Release 机制

```cpp
virtual void Retain();
virtual void Release();
```

- Retain()：对象被某处显式持有.
- Release()：显式引用释放.
- 当 _ref == 0 时, 对象释放.

#### AutoRelease 机制

```cpp
void AutoRelease();                      
```

- AutoRelease()：将对象委托给当前线程的自动释放池管理.
- 自动释放池在每个服务帧尾或析构时调用对象的 Release 方法.

运行机制:
1. 当前线程查找 LLBC_AutoReleasePoolStack, 将对象加入栈顶的自动释放池.
2. _autoRef++.
3. service 帧尾或析构时, 逐一调用池中对象的 Release() 方法. 实现对象的自动释放.

#### LLBC_Object vs std::shared_ptr / std::unique_ptr

**LLBC_Object:**
- 对象自己携带引用计数
- 所有权是概念性的（由调用者约定）
- 生命周期 = 显式 Retain / Release + 可选 AutoRelease 延迟释放

**std::shared_ptr:**
- 引用计数在控制块中
- 所有权由类型系统强制
- 生命周期完全由“还有没有人持有”决定
- 析构条件: 引用计数为 0

**std::unique_ptr:**
- 唯一所有权
- 生命周期 = 作用域
- 析构点完全确定: 离开作用域