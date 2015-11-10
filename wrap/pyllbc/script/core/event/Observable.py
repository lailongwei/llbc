# -*- coding: utf-8 -*-

from weakref import ref
from types import MethodType

import llbc

class pyllbcObservable(object):
    """
    llbc library Observable class encapsulation.
    """
    # Observer type enumeration.
    # Callable: Callable observer(not include MethodType and obj.update() specific method.
    # Method: MethodType observer(must establish special weakref object to hold the observer.
    ObserverType = pyllbcEnum('Callable', 'Method')

    # Lazy operation type enumeration.
    LazyOpType = pyllbcEnum('Add', 'Remove', 'RemoveAll', 'Notify', 'Invalid')

    class WrappedObserver(object):
        """
        Wrapped observer class encapsulation.
        """
        def __init__(self, ob):
            ob_type = pyllbcObservable.ObserverType
            if callable(ob):
                if type(ob) == MethodType:
                    if not ob.__self__:
                        raise llbc.error('observer is MethodType, but method unbound')
                    self.type = ob_type.Method
                    self.ref = ref(ob.__self__)
                    self.func = ob.im_func
                else:
                    self.type = ob_type.Callable
                    self.ref = ref(ob)
            else:
                raise llbc.error('observer not callable, observer: {}'.format(ob))
            
        def __call__(self, observable, *args, **kwargs):
            """
            Callable method impl.
            """
            ref_obj = self.ref()
            if ref_obj is None:
                return False

            ob_type = pyllbcObservable.ObserverType
            if self.type == ob_type.Method:
                self.func(ref_obj, observable, *args, **kwargs)
            else:
                ref_obj(observable, *args, **kwargs)

            return True

        def __nonzero__(self):
            """
            Makesure this observer is alive.
            """
            return True if self.ref() is not None else False

        def is_this(self, other):
            WrappedOb = pyllbcObservable.WrappedObserver
            if not isinstance(other, WrappedOb):
                other = WrappedOb(other)

            other_ref = other.ref()
            if other_ref is None:
                return False

            self_ref = self.ref()
            if self_ref is None:
                return False

            if other.type != self.type:
                return False
            elif self_ref is not other_ref:
                return False

            ob_type = pyllbcObservable.ObserverType
            if self.type == ob_type.Method:
                return self.func is other.func
            else:
                return True

    class LazyOp(object):
        def __init__(self):
            self.op_type = pyllbcObservable.LazyOpType.Invalid
            self.observer = None
            self.ntf_data = None

    def __init__(self):
        self.__notifying = 0
        self.__observers = []
        self.__lazy_ops = []

    def __contains__(self, other):
        WrappedOb = pyllbcObservable.WrappedObserver
        if not isinstance(other, WrappedOb):
            other = WrappedOb(other)

        for saved_wrapped_ob in self.__observers:
            if saved_wrapped_ob.is_this(other):
                return True

        add_times = 0
        op_type = pyllbcObservable.LazyOpType
        for lazy_op in self.__lazy_ops:
            if lazy_op.op_type == op_type.Add and lazy_op.observer.is_this(other):
                add_times += 1
            elif lazy_op.op_type == op_type.Remove and other.is_this(lazy_op.observer):
                add_times -= 1
            elif lazy_op.op_type == op_type.RemoveAll:
                add_times = 0

        return add_times > 0

    def add_observer(self, ob):
        """
        Add observer.
        """
        WrappedOb = pyllbcObservable.WrappedObserver
        if isinstance(ob, WrappedOb):
            wrapped_ob = ob
        else:
            wrapped_ob = WrappedOb(ob)
        if wrapped_ob in self:
            return

        if not self.__notifying:
            self.__observers.append(wrapped_ob)
        else:
            lazy_op = pyllbcObservable.LazyOp
            op_type = pyllbcObservable.LazyOpType

            lazy_op = lazy_op() 
            lazy_op.op_type = op_type.Add
            lazy_op.observer = wrapped_ob

            self.__lazy_ops.append(lazy_op)

    def remove_observer(self, ob):
        """
        Remove observer.
        """
        if not self.__notifying:
            wrapped_ob = pyllbcObservable.WrappedObserver(ob)

            observers = self.__observers
            for i in range(len(observers) -1, -1, -1):
                saved_wrapped_ob = observers[i]
                if not saved_wrapped_ob or \
                        saved_wrapped_ob.is_this(wrapped_ob):
                    del self.__observers[i]
        else:
            lazy_op = pyllbcObservable.LazyOp
            op_type = pyllbcObservable.LazyOpType

            lazy_op = lazy_op()
            lazy_op.observer = ob
            lazy_op.op_type = op_type.Remove
            self.__lazy_ops.append(lazy_op)

    def remove_all_observers(self):
        """
        Remove all observers.
        """
        if not self.__notifying:
            del self.__observers[:]
        else:
            lazy_op = pyllbcObservable.LazyOp
            op_type = pyllbcObservable.LazyOpType

            lazy_op = lazy_op()
            lazy_op.op_type = op_type.RemoveAll
            self.__lazy_ops.append(lazy_op)

    def notify(self, data=None):
        """
        Notify all observers
        """
        if not self.__notifying:
            self.__notifying += 1
            
            will_del_indices = []
            observers = self.__observers
            for idx, observer in enumerate(observers):
                if not observer(self, data):
                    will_del_indices.append(idx)

            for idx in xrange(len(will_del_indices) - 1, -1, -1):
                del observers[will_del_indices[idx]]

            self.__notifying -= 1
            self.__process_hangup_ops()
        else:
            lazy_op_type = pyllbcObservable.LazyOpType

            lazy_op = pyllbcObservable.LazyOp()
            lazy_op.op_type = lazy_op_type.Notify
            lazy_op.ntf_data = data
            self.__lazy_ops.append(lazy_op)

    def __process_hangup_ops(self):
        """
        Internal method, using to process hangup operations.
        """
        if not self.__lazy_ops:
            return None

        op_type = pyllbcObservable.LazyOpType

        lazy_ops = self.__lazy_ops
        self.__lazy_ops = []

        for lazy_op in lazy_ops:
            if lazy_op.op_type == op_type.Add:
                self.add_observer(lazy_op.observer)
            elif lazy_op.op_type == op_type.Remove:
                self.remove_observer(lazy_op.observer)
            elif lazy_op.op_type == op_type.RemoveAll:
                self.remove_all_observers()
            elif lazy_op.op_type == op_type.Notify:
                self.notify(lazy_op.ntf_data)

llbc.Observable = pyllbcObservable

