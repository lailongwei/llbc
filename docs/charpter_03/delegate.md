# 委托 - LLBC_Delegate

## 概述

LLBC_Delegate 是通用委托(Delegate)封装类, 用于实现类型安全的回调机制.它是一个轻量级的函数包装器, 支持将各种可调用对象(函数指针, 成员函数, Lambda 表达式等)统一封装为委托对象.

核心特性:
1. 类型安全: 编译期强类型检查, 避免类型错误.
2. 零开销抽象: C 函数指针直接调用, 无虚函数开销.
3. 支持多种可调用对象: 普通函数, 成员函数, Lambda 表达式, std::function.
4. 完整的值语意: 支持拷贝, 移动和赋值操作.
5. 空值语意: 可赋值为 nullptr, 支持空委托检查.

设计目标:
1. 统一回调接口: 将不同类型的可调用对象统一为相同的接口
2. 成员函数绑定: 简化对象成员函数的绑定, 避免手动使用 std::bind
3. 性能优化: 对 C 函数指针提供零开销的直接调用路径

## 使用示例

### 常规用法

  1. 绑定普通函数
      ```cpp
      void PrintMessage(const std::string& msg) {
          std::cout << "Message: " << msg << std::endl;
      }
      
      // 方式1: 构造时绑定
      LLBC_Delegate<void(const std::string&)> deleg1(&PrintMessage);
      deleg1("Hello from delegate");
      
      // 方式2: 赋值绑定
      LLBC_Delegate<void(const std::string&)> deleg2;
      deleg2 = &PrintMessage;
      deleg2("Hello again");
      ```
  2. 绑定成员函数
      ```cpp
      class GameLogic {
      public:
          void OnTimeout(LLBC_Timer* timer) {
              std::cout << "Timer fired, ID: " << timer->GetTimerId() << std::endl;
          }
      };
     
      GameLogic logic;
      LLBC_Timer timer(LLBC_Delegate<void(LLBC_Timer*)>(&logic, &GameLogic::OnTimeout));
      ```
     
  3. 绑定 Lambda 表达式
      ```cpp
      LLBC_Delegate<void(int)> deleg = [](int value) {
          std::cout << "Lambda called with value: " << value << std::endl;
      };
      deleg(42);
      ```
     
  4. 空委托检查与条件调用
      ```cpp
      void SafeCallback(LLBC_Delegate<void()>& callback) {
          // 方式1: bool 转换检查
          if (callback) {
              callback();
          }
          
          // 方式2: nullptr 比较
          if (callback != nullptr) {
              callback();
          }
          
          // 方式3: 重置为空
          callback = nullptr;
      }
      ```
   
## 设计与实现

**双路存储策略**, 任意时刻只有一个有效(通过构造/赋值时的初始化控制):
  1. CFunc: 存储 C 函数指针, 直接调用, 无额外开销.
  2. StlFunc: 对象指针和成员函数指针可绑定为 std::function, 支持复杂调用对象(Lambda, std::bind 等).

```
+---------------------+
|  LLBC_Delegate      |
|---------------------|
| - CFunc _cfunc      |  ← 策略1: C 函数指针
| - StlFunc _stlFunc  |  ← 策略2: STL 函数对象
|---------------------|
| + operator()(...)   |  ← 统一接口
+---------------------+
```

**模板特化**, 通过先声明, 再偏特化定义的方式, 将 LLBC_Delegate 严格限制为函数签名式使用: `LLBC_Delegate<ReturnType(ParamTypes...)>`.
利用 C++ 函数类型作为模板参数, 承载: 返回值, 参数列表和调用语意(const / noexcept / rel-qualifier).
