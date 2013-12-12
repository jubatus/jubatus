# -*- coding: utf-8 -*-

import os
import re
from cStringIO import StringIO
import time

from jubatest import *
from jubatest.unit import JubaSkipTest
from jubatest.remote import SyncRemoteProcess
from jubatest.process import LocalSubprocess
from jubatest.logger import log

class ClientGoogleTestBase():
    @classmethod
    def init_test(cls):
        # Collect all test cases
        args = [ cls.test_program, '--gtest_list_tests' ]
        proc = LocalSubprocess(args)
        proc.start()
        returncode = proc.wait()
        if returncode != 0:
            raise JubaSkipTest('%s cannot list testcases' % cls.test_program)

        # read input
        stri = StringIO(proc.stdout)
        cls.testcase = []
        current_test = None
        re_test = re.compile('^([a-zA-Z0-9_]+\.)')
        re_testcase = re.compile('^  ([a-zA-Z0-9_]+)')
        while True:
            line = stri.readline()
            if line == '': break
            if line.find('Running main') != -1: continue
            match = re_test.match(line)
            if match:
                current_test = match.group(1)

            match = re_testcase.match(line)
            if match and current_test:
                cls.testcase.append('%s%s' % (current_test, match.group(1)))

    @classmethod
    def generateTests(cls, env):
        for service, program in env.get_param('CPP_GTEST').items():
            service, program
            cls.service = service
            cls.test_program = program
            cls.init_test()
            for test in cls.testcase:
                yield cls.gtest, service, test

    def gtest(self, service, test):
        args = [ self.test_program, '--gtest_filter=%s' % test ]
        env = { 'JUBATUS_HOST': self.client_node.get_host(),
                'JUBATUS_PORT': str(self.target.get_host_port()[1]),
                'JUBATUS_CLUSTER_NAME': self.name }
        env.update(os.environ)
        proc = LocalSubprocess(args, env)
        proc.start()
        returncode = proc.wait()
        # Report gtest result when error occured
        self.assertEqual(0, returncode, proc.stdout)


class ClientStandaloneTest(JubaTestCase, ClientGoogleTestBase):
    @classmethod
    def setUpCluster(cls, env):
        cls.env = env
        if not cls.service:
            raise JubaSkipTest('CPP_GTEST parameter is not set')

        if not cls.test_program:
            raise JubaSkipTest('CPP_GTEST parameter is not set')

        cls.server1 = env.server_standalone(env.get_node(0), cls.service, default_config(cls.service))
        cls.target = cls.server1
        cls.name = ''
        cls.client_node = env.get_node(0)

    def setUp(self):
        self.server1.start()

    def tearDown(self):
        self.server1.stop()

class ClientDistributedTest(JubaTestCase, ClientGoogleTestBase):
    @classmethod
    def setUpCluster(cls, env):
        cls.env = env
        if not cls.service:
            raise JubaSkipTest('CPP_GTEST parameter is not set')

        if not cls.test_program:
            raise JubaSkipTest('CPP_GTEST parameter is not set')

        cls.node0 = env.get_node(0)
        cls.cluster = env.cluster(cls.service, default_config(cls.service))
        cls.name = cls.cluster.name

    def setUp(self):
        self.server1 = self.env.server(self.node0, self.cluster)
        self.server2 = self.env.server(self.node0, self.cluster)
        self.keeper1 = self.env.keeper(self.node0, self.service)
        self.target = self.keeper1
        self.client_node = self.env.get_node(0)
        for server in [self.keeper1, self.server1, self.server2]:
            server.start()
        self.keeper1.wait_for_servers(self.server1, self.server2)

    def tearDown(self):
        for server in [self.keeper1, self.server1, self.server2]:
            server.stop()
