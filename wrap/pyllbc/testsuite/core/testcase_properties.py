# -*- coding: utf-8 -*-
"""
Property testcase.
"""


import os
from os import path as op

from llbc import Bundle, output
from llbc import TestCase
from llbc import Properties


class PropertiesTest(TestCase):
    def run(self, *args, **kwargs):
        print 'Properties test:'

        # Construct content.
        print 'Construct properties content ...'
        content = 'llbc.test.intVal=       30 # The integer value\n'
        content += 'llbc.test.floatVal    =3.543    # The float value\n'
        content += r'llbc.test.strVal=\#\#\#==Hello World==\#\## The string value####'
        print 'Done!, content value: '
        print content

        # Load from string.
        print 'Load properties from above content...'
        properties = Properties.load_from_string(content)
        print 'Done, properties:{}\n'.format(properties)

        # Read from properties.
        print 'Read from properties'
        print 'llbc.test.intVal value:{}'.format(properties['llbc']['test']['intVal'])
        print 'llbc.test.floatVal:{}'.format(properties['llbc']['test']['floatVal'])
        print 'llbc.test.strVal:{}'.format(properties['llbc']['test']['strVal'])

        # Save to content.
        print 'Save to string content...'
        content = Properties.save_to_string(properties)
        print 'Done!, content:{}'.format(content)

        # To file.
        # We use main bundle to file path.
        bundle = Bundle.mainbundle()
        prop_file_path = op.join(bundle.path, 'test_cfg.cfg')
        print 'Save to file, to file: {}'.format(prop_file_path)
        Properties.save_to_file(properties, prop_file_path)
        print 'Done!'

        # From file
        print 'Load from file: {}'.format(prop_file_path)
        properties = Properties.load_from_file(prop_file_path)
        print 'Done! llbc.test.intVal: {}'.format(properties['llbc']['test']['intVal'])
        print 'llbc.test.floatVal:{}'.format(properties['llbc']['test']['floatVal'])
        print 'llbc.test.strVal:{}'.format(properties['llbc']['test']['strVal'])

        print 'All test done!'
        print 'Press any key to continue ...'
        raw_input()

        os.remove(prop_file_path)

        return 0
