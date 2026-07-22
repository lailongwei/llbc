---
layout: default
title: 对象容器 Array 与 Dictionary
---

# 对象容器 Array 与 Dictionary

`LLBC_Array` 与 `LLBC_Dictionary` 是 llbc objbase 子系统中的两个内置容器，专门承载
`LLBC_Object*` 指针，并与引用计数自动协作：插入时容器为元素执行 `Retain()`，移除时执行
`Release()`。两者本身也继承自 `LLBC_Object`，因此可互相嵌套并参与自动释放池。如果业务代码
不依赖 `LLBC_Object` 继承体系，推荐优先使用 STL 容器。

## 引用计数语义

容器持有元素的强引用；插入后调用一次 `Release()` 是最常见的所有权转移写法。

```cpp
LLBC_Array arr;

LLBC_Object *obj = new TestObj;   // ref = 1
arr.PushBack(obj);                // 容器 Retain，ref = 2
obj->Release();                   // 放弃自身引用，ref = 1（容器持有）

// Clear() 或 arr 析构时自动 Release 所有元素
arr.Clear();                      // ref → 0，TestObj 被销毁
```

<div class="callout warning" markdown="1">
若插入后 **不** 调用 `Release()`，容器销毁后对象仍留有一个外部引用，导致内存泄漏。
始终保持"插入即转移"的所有权约定。
</div>

## LLBC_Array：增删元素

`PushBack` / `PushFront` 在两端追加；`Insert(Iter, Obj*)` 在迭代器位置之前插入；
`Erase` 支持按迭代器、按下标或按范围删除，均自动释放被移除的元素。

```cpp
LLBC_Array arr;

// 尾部 / 头部追加
LLBC_Object *a = new TestObj; arr.PushBack(a);  a->Release();
LLBC_Object *b = new TestObj; arr.PushFront(b); b->Release();

// 在 Begin()+1 处插入
LLBC_Object *c = new TestObj;
arr.Insert(arr.Begin() + 1, c);
c->Release();

// 按迭代器删除（自动 Release 元素）
arr.Erase(arr.Begin() + 1);

// 按下标删除
arr.Erase(0);

// 删除区间 [Begin, End)——等效于 Clear()
arr.Erase(arr.Begin(), arr.End());
```

<div class="callout note" markdown="1">
`Erase(Obj *o, bool releaseObj)` 重载可控制是否释放对象（传 `false` 时仅从容器移除，
不调用 `Release()`），适用于需要手动接管对象生命周期的场景。
</div>

## LLBC_Array：迭代与按索引访问

提供正向迭代器 `Iter`/`ConstIter` 与反向迭代器 `ReverseIter`/`ConstReverseIter`；
下标运算符 `[]` 支持随机访问。

```cpp
// 正向迭代
for (LLBC_Array::Iter it = arr.Begin(); it != arr.End(); ++it)
    std::cout << (*it)->ToString();    // *it 是 LLBC_Object*&

// 反向迭代
for (LLBC_Array::ReverseIter it = arr.ReverseBegin();
     it != arr.ReverseEnd(); ++it)
    std::cout << (*it)->ToString();

// 随机访问
std::cout << arr[2]->ToString();
std::cout << arr.ObjectAtIndex(2)->ToString();   // 等价写法

// 取子集（返回新 LLBC_Array*，调用方负责 delete）
LLBC_Array::IndexSet idxs = {1, 3};
LLBC_Array *sub = arr.ObjectsAtIndexs(idxs);
// ... 使用后 delete sub;
```

<div class="callout note" markdown="1">
`LLBC_Array` 只提供 `Begin()` / `End()`（首字母大写），不满足 C++ range-for 协议，
无法直接用 `for (auto &x : arr)` 语法。
</div>

## LLBC_Array：Foreach 与 Sort

`Foreach` 接收返回 `bool` 的 callable（返回 `false` 提前中止遍历）；
`Sort` 接收与 `std::sort` 同语义的二元比较 callable：`fn(a, b)` 返回 `true` 表示 `a` 排在 `b` 前面。

```cpp
struct PrintFn {
    bool operator()(LLBC_Object *obj) {
        std::cout << obj->ToString() << "\n";
        return true;   // false 则中止
    }
};

struct AscendFn {
    bool operator()(LLBC_Object *a, LLBC_Object *b) {
        // 转型到具体子类再比较
        return static_cast<MyObj *>(a)->GetKey() <
               static_cast<MyObj *>(b)->GetKey();
    }
};

PrintFn pf; AscendFn af;
arr.Foreach(pf);             // 全量遍历
arr.Sort(af);                // 升序排序
arr.Foreach(pf);             // 确认结果
```

## LLBC_Dictionary：增删查

`LLBC_Dictionary` 以 `int` 或 `LLBC_String` 作键，底层是哈希桶 + 双向链表。
**同一实例允许 int 键与字符串键共存**。`Insert` / `Erase` / `Find` 均提供两种键类型重载，
`Insert` 成功返回 `LLBC_OK`，键重复返回 `LLBC_FAILED`。

```cpp
LLBC_Dictionary dict;   // 桶大小由 LLBC_CFG_OBJBASE_DICT_DFT_BUCKET_SIZE 决定

// 插入 int 键
LLBC_Object *o1 = new TestObj; dict.Insert(1, o1); o1->Release();

// 插入字符串键
LLBC_Object *o2 = new TestObj; dict.Insert("hello", o2); o2->Release();

// 查找（不存在时返回 End()）
LLBC_Dictionary::Iter it = dict.Find(1);
if (it != dict.End())
    std::cout << it.Obj()->ToString();

// 按键删除（返回 LLBC_OK / LLBC_FAILED）
dict.Erase(1);
dict.Erase("hello");

// 按迭代器删除
LLBC_Dictionary::Iter it2 = dict.Find("world");
if (it2 != dict.End())
    dict.Erase(it2);
```

<div class="callout note" markdown="1">
`operator[](int)` / `operator[](LLBC_String)` 均返回 `const LLBC_Object*`，仅用于只读访问。
需要替换已有元素时请用 `Replace(key, obj)`：键存在则替换并自动 Release 旧对象，
键不存在则自动插入（等同于 `Insert`），始终返回 `LLBC_OK`（`o` 为 null 时返回 `LLBC_FAILED`）。
</div>

## LLBC_Dictionary：迭代与 Foreach

遍历按**插入顺序**（双向链表决定）；`Foreach` 的 callable 参数是 `LLBC_DictionaryElem*`，
可同时读取键与值。

```cpp
struct PrintElem {
    void operator()(LLBC_DictionaryElem *elem) {
        if (elem->IsIntKey())
            std::cout << "int  key=" << elem->GetIntKey();
        else
            std::cout << "str  key=" << elem->GetStrKey();
        std::cout << "  val=" << elem->GetObject()->ToString() << "\n";
    }
};

PrintElem pe;
dict.Foreach(pe);

// 手动反向迭代；*it 是 LLBC_Object*&
for (LLBC_Dictionary::ReverseIter it = dict.ReverseBegin();
     it != dict.ReverseEnd(); ++it)
    std::cout << (*it)->ToString();
```

## 调整哈希桶大小

初始化后可通过 `SetHashBucketSize` 重新哈希，桶越大冲突越少，查找越快，但内存占用增加。

```cpp
// 建议选质数以减少哈希冲突
dict.SetHashBucketSize(101);
```

## 与 STL 容器的取舍

| 场景 | 建议 |
|------|------|
| 元素是 `LLBC_Object` 子类，需自动释放池、`Clone` 或 `Serialize` 等 objbase 特性 | `LLBC_Array` / `LLBC_Dictionary` |
| 纯值类型或非侵入式对象，追求最佳性能 | `std::vector` / `std::unordered_map` |
| 需要 int 键与字符串键共用同一容器 | `LLBC_Dictionary` |
| 需要 range-for / 标准算法（`std::sort` 等） | STL 容器 |

<div class="callout important" markdown="1">
两个容器的赋值运算符均被 `LLBC_DISABLE_ASSIGNMENT` 宏禁用，无法按值复制。
需要副本时请调用 `Clone()`（返回 `LLBC_Object*`，需强转后使用）。
</div>

## 参照

- 头文件：`llbc/include/llbc/core/objbase/Array.h`
- 头文件：`llbc/include/llbc/core/objbase/Dictionary.h`
- 头文件：`llbc/include/llbc/core/objbase/Object.h`（引用计数基类）
- func_test 示例：`tests/func_test/core/objbase/FuncTest_Core_ObjBase_Array.cpp`
- func_test 示例：`tests/func_test/core/objbase/FuncTest_Core_ObjBase_Dictionary.cpp`

## 下一步

- [LLBC_Object 与引用计数](object.md) — 理解 `Retain` / `Release` / `AutoRelease` 生命周期
- [对象池](objpool.md) — 高频对象的池化分配方案
