# -*- coding: utf-8 -*-

from __future__ import absolute_import, division, print_function, unicode_literals

from unittest import TestCase


class BinaryLengthExtractor(object):
    # Example of Python-based binary_types.

    @classmethod
    def create(cls, param):
        return cls()

    def extract(self, key, value):
        """
        Returns the length of the given binary data.

        The key is typed as ``str`` (unicode).
        The value is typed as ``bytes``.

        This method should return list of key-values (pair of feature key
        as ``str`` and its value as number).
        """
        return [("length_{0}".format(key), len(value))]


class BinaryLengthExtractorTest(TestCase):
    def test_extract(self):
        ex = BinaryLengthExtractor.create({})
        self.assertEqual([('length_v', 9)], ex.extract('v', b'XXXX-XXXX'))
