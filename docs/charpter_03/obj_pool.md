# 对象池 - LLBC_ObjPool

## 简述
LLBC对象池的核心目的是**提升对象创建及销毁的性能**，通过对象池技术，可以极大减少内存分配及释放的开销，从而提升系统整体性能。

### 特性
1. **类型安全的对象池(Typed Pool)**: 与"一个 pool 管所有对象"的简单设计不同，LLBC 采用每个类型一个 TypedObjPool:
    - 不同类型对象大小不同，避免统一 pool 的内存浪费.
    - 对象复用逻辑可以按类型定制.
    - 支持类型级统计、回收、销毁顺序控制. 
2. **自动对象复用(Reuse Detection)**: 通过编译期反射(SFINAE)检测对象是否支持复用, 如果对象有以下方法之一:
   - clear / Clear.
   - reset / Reset.
   - reuse / Reuse.
   - 或是 STL 容器(如 unordered_map).
   
   则对象释放时不会析构, 而是调用对应的"重置方法"并回收到池中. 因为是在编译器确定, 因此没有虚函数, RTTI 等开销.
3. **可选对象池反射(Pool Reflection)**: 对象可以选择感知自己来自哪个对象池:
   1. 方式一: 继承自 LLBC_PoolObject 基类.
   2. 方式二: 实现 `GetTypedObjPool() / SetTypedObjPool()` 方法, 适合轻量化非继承形式对象池管理.
4. **线程安全可选(Thread Safety Optional)**: 对象池可选线程安全模式.
5. **支持多种回收风格**:
   1. 对象池显式回收: `Obj *obj = pool.Acquire<Obj>(); pool.Release(obj);`.
   2. 自动回收(RAII 风格): `auto obj = pool.AcquireGuarded<Obj>();`.
   3. 全局回收: `LLBC_ObjReflector::Recycle(obj);`.