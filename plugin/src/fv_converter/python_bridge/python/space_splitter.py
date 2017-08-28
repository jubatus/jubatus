# -*- coding: utf-8 -*-

from __future__ import absolute_import, division, print_function, unicode_literals

from unittest import TestCase


class SpaceSplitter(object):
    # Example of Python-based string_types.

    @classmethod
    def create(cls, param):
        return cls()

    def split(self, text):
        """
        Splits the input text with space.

        The input text is typed as ``str``.
        This method should return list of boundaries (pair of beginning
        position and length), e.g., ``[ (0, 1), (3, 4) ]``.
        """

        result = []
        begin = 0
        length = 0
        for (pos, char) in enumerate(text):
            if char == ' ':
                if 0 < length:
                    result.append((begin, length))
                    length = 0
                begin = pos + 1
            else:
                length += 1
        if length != 0:
            result.append((begin, length))

        return result


class SpaceSplitterTest(TestCase):
    def test_split(self):
        ex = SpaceSplitter.create({})
        sentence = b' hi  jubatus !'
        # pos:       0123456789ABCD
        self.assertEqual([(1, 2), (5, 7), (13, 1)], ex.split(sentence))
