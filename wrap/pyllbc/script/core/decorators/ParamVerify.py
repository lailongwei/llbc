# -*- coding: utf-8 -*-

import inspect
from re import match
from functools import wraps
from types import ClassType, TypeType

from llbc import error as error

def pyllbc_varify_param(*varargs, **kwargs):
    """
    Parameters varify decorator.
    """
    varargs = map(_to_standard_condition, varargs)
    kwargs = dict((k, _to_standard_condition(kwargs[k])) for k in kwargs)

    def generator(func):
        arg_names, vararg_name, kw_name = inspect.getargspec(func)[:3]
        validator = _get_call_args(arg_names, vararg_name, kw_name, varargs, kwargs)

        @wraps(func)
        def wrapper(*call_varargs, **call_kwargs):
            call_args = _get_call_args(arg_names, vararg_name, kw_name, call_varargs, call_kwargs)
            k, item = None, None
            try:
                for k in validator:
                    if k == vararg_name:
                        for item in call_args[k]:
                            assert validator[k](item)
                    elif k == kw_name:
                        for item in call_args[k].itervalues():
                            assert validator[k](item)
                    else:
                        item = call_args[k]
                        assert validator[k](item)
            except:
                raise error('{}() parameter validation failed, param: {}, value: {}({})'
                        .format(func.func_name, k, item, item.__class__.__name__))

            return func(*call_varargs, **call_kwargs)
        return wrapper
    return generator

def pyllbc_null_ok(cls, cond=None):
    """
    Decorator to let condition can accept None value.
    """
    return lambda x: x is None or _to_standard_condition((cls, cond))(x)

def pyllbc_multi_type(*conds):
    """
    Decorator to specific multi conditions/types, as long as one cond/class check pass, if return True.
    """
    validators = map(_to_standard_condition, conds)
    def validate(x):
        for v in validators:
            if v(x):
                return True
            return False

    return validate

llbc.varify_param = pyllbc_varify_param
llbc.null_ok = pyllbc_null_ok
llbc.multi_type = pyllbc_multi_type

def _to_standard_condition(cond):
    """
    Decorator to convert all check conditions/classes to check functions.
    """
    if type(cond) in (ClassType, TypeType):
        return lambda x: isinstance(x, cond)
    elif isinstance(cond, (tuple, list)):
        cls, cond = cond[:2]
        if cond is None:
            return _to_standard_condition(cls)
        elif cls in (str, unicode) and cond[0] == cond[-1] == '/':
            return lambda x: (isinstance(x, cls) and match(cond[1:-1], x) is not None)

        return lambda x: isinstance(x, cls) and eval(cond)
    return cond

def _get_call_args(arg_names, vararg_name, kw_name, varargs, kwargs):
    """
    Decorator to integrate parameter names and values.
    """
    call_dict = {}
    varargs = tuple(varargs)
    kwargs = dict(kwargs)

    call_varargs = None
    var_arg_cnt = len(varargs)
    arg_names_cnt = len(arg_names)

    if arg_names_cnt <= var_arg_cnt:
        for n, arg_name in enumerate(arg_names):
            call_dict[arg_name] = varargs[n]

        call_carargs = varargs[-(var_arg_cnt - arg_names_cnt):]
    else:
        for n, var in enumerate(varargs):
            call_dict[arg_names[n]] = var

        for arg_name in arg_names[-(arg_names_cnt - var_arg_cnt):]:
            if arg_name in kwargs:
                call_dict[arg_name] = kwargs.pop(arg_name)

        call_varargs = ()


    if vararg_name is not None:
        call_dict[vararg_name] = call_varargs

    if kw_name is not None:
        call_dict[kw_name] = kwargs

    call_dict.update(kwargs)

    return call_dict

