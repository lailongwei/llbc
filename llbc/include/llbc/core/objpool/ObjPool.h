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

#pragma once

#include "llbc/core/rapidjson/json.h"

// Disable some warnings.
#if LLBC_TARGET_PLATFORM_WIN32
 #pragma warning(push)
 #pragma warning(disable:4200)
#endif // LLBC_TARGET_PLATFORM_WIN32

__LLBC_NS_BEGIN

// Pre-Declare: LLBC_ObjPool/LLBC_TypedObjPool.
class LLBC_ObjPool;

template <typename Obj>
class LLBC_TypedObjPool;

class LLBC_Object;

/**
 * \brief The pool object base class encapsulation.
 */
class LLBC_EXPORT LLBC_PoolObj
{
public:
    /**
     * Ctors & Dtor(Copy/Move construct skip _typedObjPool assignment).
     */
    LLBC_PoolObj(): _typedObjPool(nullptr) {  }
    LLBC_PoolObj(const LLBC_PoolObj &other): _typedObjPool(nullptr) {  }
    LLBC_PoolObj(LLBC_PoolObj &&other) noexcept: _typedObjPool(nullptr) {  }
    virtual ~LLBC_PoolObj() = default;

    /**
     * Get typed object pool.
     * @return void * - the typed object pool.
     */
    void *GetTypedObjPool() const { return _typedObjPool; }

    /**
     * Set typed object pool.
     * @param[in] typedObjPool - the typed object pool.
     */
    void SetTypedObjPool(void *typedObjPool) { _typedObjPool = typedObjPool; }

public:
    // Assignment supported(skip _typedObjPool assignment).
    LLBC_PoolObj &operator=(const LLBC_PoolObj &other) { return *this; }
    LLBC_PoolObj &operator=(LLBC_PoolObj &&other) noexcept { return *this; }

private:
    void *_typedObjPool;
};

/**
 * \brief The object reflector encapsulation.
 * Reflection support detail:
 * - Object new/delete:
 *   - New(): new object.
 *   - Delete(): delete object.
 *
 * - Object reuse:
 *   - IsReusable(constexpr): Check object reusable.
 *   - Reuse(): Reuse object.
 *
 * - Object pool reflection(can hold ObjectPool * or not):
 *   - IsSupportObjectPoolReflection(constexpr): Check object supported object pool reflection or not.
 *   - GetObjectPool(): Get object pool.
 *   - SetObjectPool(): Set object pool.
 *
 * - [Advance] Stripe control:
 *   - GetStripeCapacity(): Get stripe capacity, in object.
 */
class LLBC_ObjReflector
{
public:
    // New.
    template <typename Obj>
    static 
    void New(void *mem) { (void)(new (mem) Obj()); }

public:
    // Delete.
    template <typename Obj>
    static
    void Delete(void *mem) { reinterpret_cast<Obj *>(mem)->~Obj(); }

public:
    // IsReusable implement.

    template <typename Obj>
    static constexpr
    typename std::enable_if<LLBC_IsTemplSpec<Obj, std::unordered_set>::value ||
                            LLBC_IsTemplSpec<Obj, std::unordered_map>::value, bool>::type
    IsReusable() { return true; }

    template <typename Obj>
    static constexpr
    typename std::enable_if<!LLBC_IsTemplSpec<Obj, std::unordered_set>::value &&
                            !LLBC_IsTemplSpec<Obj, std::unordered_map>::value, bool>::type
    IsReusable() { return IsReusableInl<Obj>(0); }

private:
    #if LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_clear
    template <typename Obj, void (Obj::*)()>
    struct clearable_type;

    template <typename Obj>
    static constexpr
    bool IsReusableInl(clearable_type<Obj, &Obj::clear> *) { return true; }
    #endif // LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_clear

    #if LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_Clear
    template <typename Obj, void (Obj::*)()>
    struct Clearable_type;

    template <typename Obj>
    static constexpr
    bool IsReusableInl(Clearable_type<Obj, &Obj::Clear> *) { return true; }
    #endif // LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_Clear

    #if LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_reset
    template <typename Obj, void (Obj::*)()>
    struct reseatable_type;

    template <typename Obj>
    static constexpr
    bool IsReusableInl(reseatable_type<Obj, &Obj::reset> *) { return true; }
    #endif // LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_reset

    #if LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_Reset
    template <typename Obj, void (Obj::*)()>
    struct Reseatable_type;

    template <typename Obj>
    static constexpr
    bool IsReusableInl(Reseatable_type<Obj, &Obj::Reset> *) { return true; }
    #endif // LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_Reset

    #if LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_reuse
    template <typename Obj, void (Obj::*)()>
    struct reusable_type;

    template <typename Obj>
    static constexpr
    bool IsReusableInl(reusable_type<Obj, &Obj::reuse> *) { return true; }
    #endif // LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_reuse

    #if LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_Reuse
    template <typename Obj, void (Obj::*)()>
    struct Reusable_type;

    template <typename Obj>
    static constexpr
    bool IsReusableInl(Reusable_type<Obj, &Obj::Reuse> *) { return true; }
    #endif // LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_Reuse

    // Some clear() method in base class STL containers reuse support, like: map/set.
    template <typename Obj, void (Obj::_Mybase::*)()>
    struct clearmethod_in_base_stl_container_type;

    template <typename Obj>
    static constexpr
    bool IsReusableInl(clearmethod_in_base_stl_container_type<Obj,
                                                              &Obj::_Mybase::clear> *)
    {
        return true;
    }

    // Unusable object reuse support: Do nothing.
    template <typename Obj>
    static constexpr
    bool IsReusableInl(...) { return false; }

public:
    // Reuse implement.

    template <typename Obj>
    static
    typename std::enable_if<LLBC_IsTemplSpec<Obj, std::unordered_set>::value ||
                            LLBC_IsTemplSpec<Obj, std::unordered_map>::value, void>::type
    Reuse(void *mem) { reinterpret_cast<Obj *>(mem)->clear(); }

    template <typename Obj>
    static
    typename std::enable_if<!LLBC_IsTemplSpec<Obj, std::unordered_set>::value &&
                            !LLBC_IsTemplSpec<Obj, std::unordered_map>::value, void>::type
    Reuse(void *mem) { ReuseInl<Obj>(mem, 0); }

private:
    #if LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_clear
    template <typename Obj>
    static void ReuseInl(void *mem, clearable_type<Obj, &Obj::clear> *)
    {
        reinterpret_cast<Obj *>(mem)->clear();
    }
    #endif // LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_clear

    #if LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_Clear
    template <typename Obj>
    static void ReuseInl(void *mem, Clearable_type<Obj, &Obj::Clear> *)
    {
        reinterpret_cast<Obj *>(mem)->Clear();
    }
    #endif // LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_Clear

    #if LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_reset
    template <typename Obj>
    static void ReuseInl(void *mem, reseatable_type<Obj, &Obj::reset> *)
    {
        reinterpret_cast<Obj *>(mem)->reset();
    }
    #endif // LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_reset

    #if LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_Reset
    template <typename Obj>
    static void ReuseInl(void *mem, Reseatable_type<Obj, &Obj::Reset> *)
    {
        reinterpret_cast<Obj *>(mem)->Reset();
    }
    #endif // LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_Reset

    #if LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_reuse
    template <typename Obj>
    static void ReuseInl(void *mem, reusable_type<Obj, &Obj::reuse> *)
    {
        reinterpret_cast<Obj *>(mem)->reuse();
    }
    #endif // LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_reuse

    #if LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_Reuse
    template <typename Obj>
    static void ReuseInl(void *mem, Reusable_type<Obj, &Obj::Reuse> *)
    {
        reinterpret_cast<Obj *>(mem)->Reuse();
    }
    #endif // LLBC_CFG_CORE_OBJPOOL_OBJ_REUSE_MATCH_METH_Reuse

    // STL containers reuse support, like: map/set.
    template <typename Obj>
    static void ReuseInl(void *mem,
                         clearmethod_in_base_stl_container_type<Obj,
                                                                &Obj::_Mybase::clear> *)
    {
        reinterpret_cast<Obj *>(mem)->clear();
    }

    // Unusable object reuse support: Do nothing.
    template <typename Obj>
    static void ReuseInl(void *mem, ...) {  }

public:
    // Object pool reflection support.
    template <typename Obj>
    static constexpr
    typename std::enable_if<std::is_base_of<LLBC_PoolObj, Obj>::value, bool>::type
    IsSupportedObjPoolReflection() { return true; }

    template <typename Obj>
    static constexpr
    typename std::enable_if<!std::is_base_of<LLBC_PoolObj, Obj>::value, bool>::type
    IsSupportedObjPoolReflection()
    {
        return IsSupportedObjPoolReflectionInl<Obj>(0);
    }

private:
    template <typename Obj,
              LLBC_TypedObjPool<Obj> *(Obj::*)() const,
              void (Obj::*)(LLBC_TypedObjPool<Obj> *)>
    struct object_pool_detectable_type;

    template <typename Obj>
    static constexpr
    bool IsSupportedObjPoolReflectionInl(object_pool_detectable_type<Obj,
                                                                     &Obj::GetTypedObjPool,
                                                                     &Obj::SetTypedObjPool> *)
    {
        return true;
    }

    template <typename Obj>
    static constexpr
    bool IsSupportedObjPoolReflectionInl(...) { return false; }

public:
    // GetTypedObjPool implement.
    template <typename Obj>
    static
    typename std::enable_if<std::is_base_of<LLBC_PoolObj, Obj>::value,
                            LLBC_TypedObjPool<Obj> *>::type
    GetTypedObjPool(void *mem)
    {
        return reinterpret_cast<LLBC_TypedObjPool<Obj> *>(
            reinterpret_cast<Obj *>(mem)->GetTypedObjPool());
    }

    template <typename Obj>
    static
    typename std::enable_if<!std::is_base_of<LLBC_PoolObj, Obj>::value,
                            LLBC_TypedObjPool<Obj> *>::type
    GetTypedObjPool(void *mem)
    {
        return GetTypedObjPoolInl<Obj>(mem, 0);
    }

private:
    template <typename Obj>
    static
    LLBC_ObjPool *GetTypedObjPoolInl(void *mem,
                                     object_pool_detectable_type<Obj,
                                                                 &Obj::GetTypedObjPool,
                                                                 &Obj::SetTypedObjPool> *)
    {
        return reinterpret_cast<Obj *>(mem)->GetTypedObjPool();
    }

    template <typename Obj>
    static constexpr
    LLBC_ObjPool *GetTypedObjPoolInl(void *mem, ...) { return nullptr; }

public:
    // SetTypedObjPool implement.
    template <typename Obj>
    static
    typename std::enable_if <std::is_base_of<LLBC_PoolObj, Obj>::value, void>::type
    SetTypedObjPool(void *mem, LLBC_TypedObjPool<Obj> *typedObjPool)
    {
        reinterpret_cast<Obj *>(mem)->SetTypedObjPool(typedObjPool);
    }

    template <typename Obj>
    static
    typename std::enable_if <!std::is_base_of<LLBC_PoolObj, Obj>::value, void>::type
    SetTypedObjPool(void *mem, LLBC_TypedObjPool<Obj> *typedObjPool)
    {
        SetTypedObjPoolInl<Obj>(mem, typedObjPool, 0);
    }

private:
    template <typename Obj>
    static void SetTypedObjPoolInl(void *mem,
                                   LLBC_TypedObjPool<Obj> *typedObjPool,
                                   object_pool_detectable_type<Obj,
                                                               &Obj::GetTypedObjPool,
                                                               &Obj::SetTypedObjPool> *)
    {
        reinterpret_cast<Obj *>(mem)->SetTypedObjPool(typedObjPool);
    }

    template <typename Obj>
    static void SetTypedObjPoolInl(void *mem,
                                   LLBC_TypedObjPool<Obj> *objPool,
                                   ...) {  }

public:
    // GetStripeCapacity.
    template <typename Obj>
    static size_t GetStripeCapacity()
    {
        return GetStripeCapacityInl<Obj>(0);
    }

private:
    template <typename Obj, size_t (Obj::*)()>
    struct stripe_capacity_detectable_type;

    template <typename Obj>
    static size_t GetStripeCapacityInl(
        stripe_capacity_detectable_type<Obj, &Obj::GetStripeCapacity> *)
    {
        #if LLBC_CUR_COMP == LLBC_COMP_GCC || LLBC_CUR_COMP == LLBC_COMP_CLANG
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wnonnull"
        #endif

        return reinterpret_cast<Obj *>(NULL)->GetStripeCapacity();

        #if LLBC_CUR_COMP == LLBC_COMP_GCC || LLBC_CUR_COMP == LLBC_COMP_CLANG
        #pragma GCC diagnostic pop
        #endif
    }

    template <typename Obj>
    static constexpr size_t GetStripeCapacityInl(...)
    {
        return LLBC_CFG_CORE_OBJPOOL_STRIPE_CAPACITY;
    }

public:
    template <typename Obj>
    static void OnTypedObjPoolCreated(LLBC_TypedObjPool<Obj> *typedObjPool)
    {
        OnTypedObjPoolCreatedInl<Obj>(typedObjPool, 0);
    }

private:
    template <typename Obj, void (Obj::*)(LLBC_TypedObjPool<Obj> *typedObjPool)>
    struct typed_obj_pool_created_ev_handler;

    template <typename Obj>
    static void OnTypedObjPoolCreatedInl(LLBC_TypedObjPool<Obj> *typedObjPool,
                                         typed_obj_pool_created_ev_handler<Obj, &Obj::OnTypedObjPoolCreated> *)
    {
        #if LLBC_CUR_COMP == LLBC_COMP_GCC || LLBC_CUR_COMP == LLBC_COMP_CLANG
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wnonnull"
        #endif

        return reinterpret_cast<Obj *>(NULL)->OnTypedObjPoolCreated(typedObjPool);

        #if LLBC_CUR_COMP == LLBC_COMP_GCC || LLBC_CUR_COMP == LLBC_COMP_CLANG
        #pragma GCC diagnostic pop
        #endif
    }

    template <typename Obj>
    static void OnTypedObjPoolCreatedInl(LLBC_TypedObjPool<Obj> *typedObjPool, ...)
    {
        // Do nothing.
    }

public:
    // Recycle object.
    template <typename Obj>
    static
    typename std::enable_if<std::is_base_of<LLBC_PoolObj, Obj>::value, void>::type
    Recycle(Obj *obj)
    {
        LLBC_ReturnIf(UNLIKELY(obj == nullptr), void());
        if constexpr (std::is_base_of_v<LLBC_Object, Obj>)
        {
            // add obj to gc-pool, if already do nothing
            obj->AutoRelease();
            return;
        }

        LLBC_TypedObjPool<Obj> *typedObjPool =
            reinterpret_cast<LLBC_TypedObjPool<Obj> *>(obj->GetTypedObjPool());
        if (typedObjPool)
            typedObjPool->Release(obj);
        else
            delete obj;
    }

    template <typename Obj>
    static
    typename std::enable_if<!std::is_base_of<LLBC_PoolObj, Obj>::value, void>::type
    Recycle(Obj *obj)
    {
        LLBC_ReturnIf(UNLIKELY(obj == nullptr), void());
        if constexpr (std::is_base_of_v<LLBC_Object, Obj>)
        {
            // add obj to gc-pool, if already do nothing
            obj->AutoRelease();
            return;
        }

        RecycleInl<Obj>(obj, 0);
    }

    template <typename Obj>
    static void RecycleX(Obj *&obj) { if (obj) { Recycle(obj); obj = nullptr; } }

private:
    template <typename Obj>
    static void RecycleInl(Obj *obj,
                           object_pool_detectable_type<Obj,
                                                       &Obj::GetTypedObjPool,
                                                       &Obj::SetTypedObjPool> *)
    {
        LLBC_TypedObjPool<Obj> *typedObjPool = obj->GetTypedObjPool();
        if (typedObjPool)
            typedObjPool->Release(obj);
        else
            delete obj;
    }

    template <typename Obj>
    static void RecycleInl(Obj *obj, ...) { delete obj; }
};

/**
 * \brief The object pool statistics format enumeration.
 */
class LLBC_ObjPoolStatFormat
{
public:
    // Use 4~7 bits as format type, 0~3 bits as specified type.
    enum ENUM
    {
        Json = 0x00,
        PrettyJson = 0x01,

        CSV = 0x10,
        CSVWithoutHead = 0x11,
    };
};

/**
 * The guarded pool object encapsulation.
 */
template <typename Obj>
class LLBC_GuardedPoolObj final
{
    /**
     * Ctors & Dtor.
     */
private:
    LLBC_GuardedPoolObj(Obj *obj, LLBC_TypedObjPool<Obj> *typedObjPool);
public:
    LLBC_GuardedPoolObj(const LLBC_GuardedPoolObj &other);
    LLBC_GuardedPoolObj(LLBC_GuardedPoolObj &&other) noexcept;
    ~LLBC_GuardedPoolObj();

public:
    /**
     * object member access operator impl.
     */
    Obj *operator->() { return _obj; }
    const Obj *operator->() const { return _obj; }

    /**
     * object dereference operator impl.
     */
    Obj &operator*() { return *_obj; }
    const Obj &operator*() const { return *_obj; }

    /**
     * Get object.
     * @return Obj * - the object pointer.
     */
    Obj *Get() { return _obj; }
    const Obj *Get() const { return _obj; }

    /**
     * Reset guarded object.
     */
    void Reset();

    /**
     * Detach object.
     * @return Obj * - the detached object pointer.
     */
    Obj *Detach() { Obj *obj = _obj; _obj = nullptr; return obj; }

    /**
     * Get typed object pool.
     * @return LLBC_TypedObjPool<Obj> * - the typed object pool.
     */
    LLBC_TypedObjPool<Obj> *GetTypedObjPool() const { return _obj ? _typedObjPool : nullptr; }

public:
    /**
     * Check guarded pool object available or not.
     */
    explicit operator bool() const { return _obj != nullptr; }

    /**
     * Copy assignment/Move assignment impl.
     */
    LLBC_GuardedPoolObj &operator=(const LLBC_GuardedPoolObj &other);
    LLBC_GuardedPoolObj &operator=(LLBC_GuardedPoolObj &&other) noexcept;

private:
    friend class LLBC_TypedObjPool<Obj>;

    mutable Obj *_obj;
    LLBC_TypedObjPool<Obj> *_typedObjPool;
};

/**
 * \brief Specific type object pool.
 */
template <typename Obj>
class LLBC_TypedObjPool final 
{
    LLBC_DISABLE_ASSIGNMENT(LLBC_TypedObjPool);
    LLBC_DISABLE_MOVE_ASSIGNMENT(LLBC_TypedObjPool);

    #pragma pack(push, 1)
    // Pre-declare object stripe structure.
    struct _ObjStripe;

     // The wrapped object structure encapsulation.
    struct _WrappedObj
    {
        // Object flags.
        union
        {
            struct
            {
                bool constructed:1; // Constructed flag.
                bool inUsing:1; // Using flag.
                uint8 reserved:6; // Reserved flags.
            } flags;
            uint8 flagsVal;
        }unFlags;

        uint16 magicNum; // Pool object magic number.

        #if LLBC_64BIT_PROCESSOR
        uint8 __unused__[5];
        #else
        uint8 __unused__[1];
        #endif

        void *typedObjPool; // Typed object pool.

        // Next object or owned object stripe.
        union
        {
            _ObjStripe *stripe; // Owned object stripe, if in using.
            _WrappedObj *nextFreeObj; // Next free object, if not in using.
        } stripeOrNextFreeObj;

        #if LLBC_64BIT_PROCESSOR
        uint8 buff[(sizeof(Obj) + 7) & (-8)]; // Object begin address, aligned to 8.
        #else
        uint8 buff[(sizeof(Obj) + 3) & (-4)]; // Object begin address, aligned to 4.
        #endif
    };

    // The object stripe structure encapsulation.
    struct _ObjStripe
    {
        uint16 cap; // Stripe capacity, in object.
        uint16 used; // Used stripe memory, in object.
        #if LLBC_64BIT_PROCESSOR
        uint8 __unused__[4]; // Uunused.
        #endif

        _WrappedObj *freeObjs; // Free objects.
        _ObjStripe *nextFreeStripe; // Next stripe, if free.

        _WrappedObj objs[0]; // objs.
    };
    #pragma pack(pop)

private:
    friend class LLBC_ObjPool;

    explicit LLBC_TypedObjPool(LLBC_ObjPool *objPool, bool threadSafe);
    ~LLBC_TypedObjPool();

public:
    // Get object pool.
    LLBC_ObjPool *GetObjPool() const { return _objPool; }

    // Acquire.
    Obj *Acquire();
    // Acquire guarded.
    LLBC_GuardedPoolObj<Obj> AcquireGuarded() { return LLBC_GuardedPoolObj<Obj>(Acquire(), this); }

    // Release.
    void Release(Obj *obj);

    // Collect.
    void Collect(bool deep);

private:
    // Release object static method.
    static void ReleaseObj_s(void *typedObjPool, void *obj);

    // Destruct typed object pool static method.
    static void Destruct_s(void *typedObjPool);

    // Collect typed object pool static method.
    static void Collect_s(void *typedObjPool, bool deep);

    // Get typed object pool statistics static method.
    static LLBC_Json::Value GetStatistics_s(void *typedObjPool,
                                            LLBC_Json::MemoryPoolAllocator<> &jsonAlloc);

    // Find free stripe.
    _ObjStripe *FindFreeStripe();
    // Delete stripe.
    void DeleteStripe(_ObjStripe *stripe);

    // Get statistics.
    LLBC_Json::Value GetStatistics(LLBC_Json::MemoryPoolAllocator<> &jsonAlloc) const;

private:
    bool _threadSafe; // Thread safe flag.
    mutable LLBC_SpinLockHandle _lock; // Typed object pool lock.

    LLBC_ObjPool *_objPool; // object pool.
    LLBC_CString _objTypeName; // Object type name(rtti name).
    std::vector<_ObjStripe *> _stripes; // Stripes.
    _ObjStripe *_freeStripes; // Free stripes.

    size_t _usingObjCount; // Using object count.
    int _reusableObjCount; // Reusable object count.

    static constexpr size_t _objOffset = offsetof(_WrappedObj, buff); // Object offset in _WrappedObj.
};

/**
 * \brief The object pool encapsulation.
 */
class LLBC_EXPORT LLBC_ObjPool final
{
    LLBC_DISABLE_ASSIGNMENT(LLBC_ObjPool);
    LLBC_DISABLE_MOVE_ASSIGNMENT(LLBC_ObjPool);

public:
    explicit LLBC_ObjPool(bool threadSafe = false);
    ~LLBC_ObjPool();

public:
    /**
     * Acquire object from pool.
     * @return Obj * - the object pointer.
     */
    template <typename Obj> Obj *Acquire() 
    {
        static_assert(
            !std::is_base_of_v<LLBC_Object, Obj>,
            "Obj can not create by ObjPool, as is derived form LLBC_Object");
        return GetTypedObjPool<Obj>()->Acquire();
    }

    /**
     * Acquire guarded object from pool.
     * @return LLBC_GuardedPoolObj<Obj> - the guarded object pool.
     */
    template <typename Obj>
    LLBC_GuardedPoolObj<Obj> AcquireGuarded() { return GetTypedObjPool<Obj>()->AcquireGuarded(); }
    /**
     * Release object.
     * @param[in] obj - the object pointer.
     */
    template <typename Obj>
    void Release(Obj *obj);

    /**
     * Collect typed object pool.
     * @param[in] deep - deep collect or not.
     */
    template <typename Obj>
    void Collect(bool deep) { GetTypedObjPool<Obj>()->Collect(deep); }

    /**
     * Collect object pool(collect all typed object pools).
     * @param[in] deep - deep collect or not.
     */
    void Collect(bool deep);

    /**
     * Get typed object pool.
     * @return LLBC_TypedObjPool<Obj> * - the typed object pool.
     */
    template <typename Obj>
    LLBC_TypedObjPool<Obj> *GetTypedObjPool();

    /**
     * Get statistics.
     * @param[in] statFmt - object pool statistic format, default is CSV format(semicolon separated).
     * @param[in] pretty  - pretty flag.
     * @return LLBC_String - the object pool statistics.
     */
    LLBC_String GetStatistics(int statFmt = LLBC_ObjPoolStatFormat::CSV) const;

    /**
     * Ensure <ObjA> deletion before <ObjB>.
     * @tparam ObjA - before delete object type.
     * @tparam ObjB - after delete object type.
     * @return int - return 0 if success, otherwise return -1.
     */
    template <typename ObjA, typename ObjB>
    int EnsureDeletionBefore();

    /**
     * Get ordered delete tree, json format.
     * @param[in] pretty - pretty flag, default is false.
     * @return LLBC_String - the ensure deletion tree.
     */
    LLBC_String GetOrderedDeleteTree(bool pretty = false) const;

    /**
     * Get name of objPool.
     * @return LLBC_String - the name of this objPool.
     */
    LLBC_String GetName() const;

     /**
     * Set name for objPool.
     * @param[in] poolName - the name of this objPool.
     */
    void SetName(const LLBC_CString &poolName);

private:
    // The wrapped TypedObjPool structure encapsulation.
    struct _WrappedTypedObjPool
    {
        LLBC_CString rttiName;
        void (*ReleaseObj)(void *, void *);
        void (*Destruct)(void *);
        void (*Collect)(void *, bool);
        LLBC_Json::Value (*GetStatistics)(void *, LLBC_Json::MemoryPoolAllocator<> &);

        uint8 typedObjPool[0];
    };

    /**
     * Delete typed object pool.
     * @param[in] wrappedTypedObjPool - the wrapped typed object pool.
     */
    void DelTypedObjPool(_WrappedTypedObjPool *wrappedTypedObjPool);

private:
    // The ordered deletion node encapsulation.
    class _OrderedDeleteNode
    {
    public:
        explicit _OrderedDeleteNode(const LLBC_CString &name);
        ~_OrderedDeleteNode();

    public:
        // Get node name.
        const LLBC_CString &GetName() const { return _name; }

        // Get root node(if _front is null, return null).
        _OrderedDeleteNode *GetRoot() const;
        // Get front node.
        _OrderedDeleteNode *GetFront() const { return _front; }
        // Get back nodes.
        const std::map<LLBC_CString, _OrderedDeleteNode *> &GetBacks() const;

        // Add node to back nodes.
        int AddBack(_OrderedDeleteNode *backNode);
        // Remove node from back nodes.
        int RemoveBack(const LLBC_CString &name);

        // Front node check.
        bool IsFront(const LLBC_CString &name) const;
        // Back node check.
        bool IsBack(const LLBC_String &name) const;

        // Get ordered delete tree.
        LLBC_Json::Value GetOrderedDeleteTree(LLBC_Json::Document &jsonDoc) const;

    private:
        LLBC_CString _name;
        _OrderedDeleteNode *_front;
        std::map<LLBC_CString, _OrderedDeleteNode *> *_backs;
    };

    // Operate ordered delete nodes.
    void OperateOrderedDeleteNodes(bool isCollect, bool deepCollect);
    // Operate one ordered delete node.
    void OperateOneOrderedDeleteNode(_OrderedDeleteNode *orderedDelNode,
                                     bool isCollect,
                                     bool deepCollect);

private:
    // Objpool name, default is 'ObjPool_<thread_id>_<safe/unsafe>_<inc-id>'
    LLBC_String _name;

    // Thread safe about variables.
    bool _threadSafe;
    mutable LLBC_SpinLockHandle _lock;

    // Typed object pools.
    std::map<LLBC_CString, _WrappedTypedObjPool *> _typedObjPools;

    // Ordered delete nodes & node tree.
    std::map<LLBC_CString, _OrderedDeleteNode *> *_orderedDeleteNodes;
    std::map<LLBC_CString, _OrderedDeleteNode *> *_orderedDeleteNodeTree;

    // ReleaseObj() method offset in _WrappedTypedObjPool.
    static constexpr size_t _releaseObjMethOffset =
        offsetof(_WrappedTypedObjPool, typedObjPool) - offsetof(_WrappedTypedObjPool, ReleaseObj);
};

__LLBC_NS_END

#include "llbc/core/objpool/ObjPoolInl.h"

// Restore some warnings.
#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(pop)
#endif // LLBC_TARGET_PLATFORM_WIN32
