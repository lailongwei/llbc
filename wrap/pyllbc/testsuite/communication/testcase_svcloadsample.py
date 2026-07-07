# -*- coding: utf-8 -*-
"""
Simple test for Service.start(pollercount, load_sample_time) load sample argument.
"""

import llbc


class SvcLoadSampleTest(llbc.TestCase):
    def run(self, arg):
        print 'Service load sample test:'

        # case 1: sampling disabled.
        svc1 = llbc.Service('svcloadsample_test_svc1')
        svc1.start()
        assert svc1.started, 'svc1 should be started with default args'
        info1 = svc1.get_recent_load_info(recent_time=1)
        assert info1 is None, \
            'svc1(load_sample_time=0) should NOT provide load info, but got: {}'.format(info1)
        print '- case 1 ok: sampling disabled, get_recent_load_info() -> None'
        svc1.stop()

        # case 2: sampling enabled.
        sample_interval = 6
        svc2 = llbc.Service('svcloadsample_test_svc2')
        svc2.start(pollercount=1, load_sample_time=sample_interval * 4)
        assert svc2.started, 'svc2 should be started with load_sample_time enabled'
        llbc.Service.schedule(schedule_time=sample_interval * 2 + 1)
        info2 = svc2.get_recent_load_info(recent_time=sample_interval * 3)
        assert info2 is not None, \
            'svc2(load_sample_time>0) should provide load info, but got None'
        print '- case 2 ok: sampling enabled, load info: {}'.format(info2)
        svc2.stop()

        print 'Service load sample test done.'
