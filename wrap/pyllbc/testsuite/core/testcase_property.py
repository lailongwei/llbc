# -*- coding: utf-8 -*-
"""
Property testcase.
"""


import os
from os import path as op

from llbc import Bundle, output
from llbc import TestCase
from llbc import Property


class PropertyTest(TestCase):
    def run(self, *args, **kwargs):
        print 'Property test:'

        # Construct content.
        print 'Construct property content ...'
        content = 'llbc.test.intVal=       30 # The integer value\n'
        content += 'llbc.test.floatVal    =3.543    # The float value\n'
        content += r'llbc.test.strVal=\#\#\#\.\.\=\=Hello World\=\=\.\.\#\## The string value####'
        print 'Done!, content value: '
        print content

        # Create property and init from above constructed content.
        print 'Load property from above content...'
        prop = Property()
        prop.from_content(content)
        print 'Done!\n'

        # Read from property
        print 'Read from property'
        intProp = prop.llbc.test.intVal
        print 'llbc.test.intVal value:{}, comments:{}'.format(
            intProp.getvalue(as_type=int), intProp.get_comments())
        floatProp = prop.llbc.test.floatVal
        print 'llbc.test.floatVal:{}, comments:{}'.format(
            floatProp.getvalue(as_type=float), floatProp.get_comments())
        print 'llbc.test.strVal:{}, comments:{}'.format(
            prop.getvalue('llbc.test.strVal'), prop.get_comments('llbc.test.strVal'))

        # To content.
        print 'convert to content...'
        content = prop.to_content()
        print 'Done!, content:'
        print content

        # To file.
        # We use main bundle to file path.
        bundle = Bundle.mainbundle()
        prop_file_path = op.join(bundle.path, 'test_cfg.cfg')

        print 'to file test, to file: {}'.format(prop_file_path)
        prop.to_file(prop_file_path)
        print 'Done!'

        # From file
        print 'Reinit property from file: {}'.format(prop_file_path)
        prop.from_file(prop_file_path)
        print 'Done! llbc.test.intVal: {}'.format(prop.llbc.test.intVal.getvalue(as_type=int))

        print 'All test done!'
        print 'Press any key to continue ...'
        raw_input()

        del prop
        os.remove(prop_file_path)

        return 0

    @staticmethod
    def __dumpprop(prop):
        for k, v in prop.iteritems():
            print k, '=', v

        print


