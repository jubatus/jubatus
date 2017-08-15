# -*- coding: utf-8 -*-

from __future__ import absolute_import, division, print_function, unicode_literals

from unittest import TestCase


class NumberMultiplier(object):
    # Example of Python-based num_types.

    @classmethod
    def create(cls, param):
        # This example uses parameter ``n`` from config file.
        # ``2`` is used when not specified.
        n = int(param.get('n', 2))
        return cls(n)

    def __init__(self, n):
        self._n = n

    def extract(self, key, value):
        """
        Returns the number multipled by the number specified in the
        config (``n``).

        The key is typed as ``str`` (unicode).
        The value is typed as ``float``.

        This method should return list of key-values (pair of feature key
        as ``str`` and its value as number).
        """
        return [(key, value * self._n)]


class NumberMultiplierTest(TestCase):
    def test_extract(self):
        ex = NumberMultiplier.create({'n': 3})
        self.assertEqual([('v', 30)], ex.extract('v', 10))
