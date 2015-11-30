# -*- coding: utf-8 -*-

from __future__ import absolute_import, division, print_function, unicode_literals

import os
import re
import time

try:
    # for Python 3.x
    from io import StringIO
except ImportError:
    # for Python 2.x
    from cStringIO import StringIO

from jubatest import *
from jubatest.unit import JubaSkipTest
from jubatest.remote import SyncRemoteProcess
from jubatest.process import LocalSubprocess
from jubatest.logger import log

class ClientGoogleTestBase():
    @classmethod
    def find_testcases(cls, test_program):
        # Collect all test cases
        args = [ test_program, '--gtest_list_tests' ]
        proc = LocalSubprocess(args)
        proc.start()
        returncode = proc.wait()
        if returncode != 0:
            raise JubaSkipTest('%s cannot list testcases' % test_program)

        # read input
        stri = StringIO(proc.stdout.decode())
        testcases = []
        current_test = None
        re_test = re.compile(r'^([a-zA-Z0-9_]+\.)')
        re_testcase = re.compile(r'^  ([a-zA-Z0-9_]+)')
        while True:
            line = stri.readline()
            if line == '': break
            if line.find('Running main') != -1: continue
            match = re_test.match(line)
            if match:
                current_test = match.group(1)

            match = re_testcase.match(line)
            if match and current_test:
                testcases.append('%s%s' % (current_test, match.group(1)))
        return testcases

    @classmethod
    def setUpCluster(cls, env):
        cls.env = env

    @classmethod
    def generateTests(cls, env):
        if env.get_param('CPP_GTEST') is None:
            raise JubaSkipTest('CPP_GTEST parameter is not set')
        for service, test_program in env.get_param('CPP_GTEST').items():
            for test in cls.find_testcases(test_program):
                yield cls.gtest, service, test_program, test

    def gtest(self, service, test_program, test):
        self.lazySetUp(service)
        args = [ test_program, '--gtest_filter=%s' % test ]
        env = { 'JUBATUS_HOST': self.client_node.get_host(),
                'JUBATUS_PORT': str(self.target.get_host_port()[1]),
                'JUBATUS_CLUSTER_NAME': self.name,
                'JUBATUS_TIMEOUT': str(5),
                'JUBATUS_SERVERS_COUNT': str(self.servers_count()),
              }
        env.update(os.environ)
        proc = LocalSubprocess(args, env)
        proc.start()
        returncode = proc.wait()
        # Report gtest result when error occured
        self.assertEqual(0, returncode, proc.stdout)

class ClientStandaloneTest(JubaTestCase, ClientGoogleTestBase):
    def servers_count(self):
        return 1

    def lazySetUp(self, service):
        self.server1 = self.env.server_standalone(self.env.get_node(0), service, default_config(service))
        self.target = self.server1
        self.name = ''
        self.client_node = self.env.get_node(0)
        self.server1.start()

    def tearDown(self):
        self.server1.stop()

"""
class ClientDistributedTest(JubaTestCase, ClientGoogleTestBase):
    def servers_count(self):
        return 2

    def lazySetUp(self, service):
        self.node0 = self.env.get_node(0)
        self.cluster = self.env.cluster(service, default_config(service))
        self.name = self.cluster.name
        self.server1 = self.env.server(self.node0, self.cluster)
        self.server2 = self.env.server(self.node0, self.cluster)
        self.keeper1 = self.env.keeper(self.node0, service)
        self.target = self.keeper1
        self.client_node = self.env.get_node(0)
        for server in [self.keeper1, self.server1, self.server2]:
            server.start()
        self.keeper1.wait_for_servers(self.server1, self.server2)

    def tearDown(self):
        for server in [self.keeper1, self.server1, self.server2]:
            server.stop()
"""
