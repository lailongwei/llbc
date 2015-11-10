# -*- coding: utf-8 -*-

__author__ = 'Longwei Lai'
__create_date__ = '2014/9/29'
__doc__ = """Just test service all decorators"""

from llbc import Service

from module_a import *
from module_b import *

import struct

def main():
    client = Service(Service.CLIENT, 'client')
    client.codec = Service.CODEC_BINARY
    client.startasclient()

    server = Service(Service.SERVER, 'server')
    server.codec = Service.CODEC_BINARY
    server.startasserver('127.0.0.1', 7788)

    client2 = Service(Service.CLIENT, 'client2')
    client2.codec = Service.CODEC_BINARY
    client2.startasclient()

    Service.schedule()


if __name__ == '__main__':
    main()
