# -*- coding: utf-8 -*-
"""
Bundle相关测试
"""


import os
from os import path as op

import llbc
from llbc import TestCase
from llbc import Bundle


class BundleTest(TestCase):
    def run(self, arg):
        self.name = 'BundleTest'
        print 'Bundle testcase:'

        # Test main bundle.
        b = Bundle.mainbundle()
        print 'Main bundle path:{}'.format(b.path)
        print 'Main bundle name:{}'.format(b.name)

        # Create new bundle.
        my_bundle_name = 'my_bundle'
        try:
            _ = Bundle(my_bundle_name)
        except llbc.error, e:
            print 'Try create my bundle, failed, reason:{}'.format(e)
            print 'We create the bundle dir...'
            os.makedirs(op.join(b.path, my_bundle_name))

        my_bundle = Bundle(my_bundle_name)
        print 'Bundle [{}] created:'.format(my_bundle.name)
        print '{} path: {}'.format(my_bundle.name, my_bundle.path)

        # We try in the my_bundle create a txt file.
        test_file_name = 'test.txt'
        test_file_path = op.join(my_bundle.path, test_file_name)
        with open(test_file_path, 'w+') as f:
            f.write('Hello llbc!\n')

        # Now, get the test file path.
        rtn_path = my_bundle.getrespath(test_file_name)
        print 'The test file[{}] in bundle path: {}'.format(test_file_name, rtn_path)

        # Cleanup test resources.
        os.remove(test_file_path)
        os.removedirs(my_bundle.path)

        print 'Press any key to continue ... '
        _ = raw_input()

        return 0
