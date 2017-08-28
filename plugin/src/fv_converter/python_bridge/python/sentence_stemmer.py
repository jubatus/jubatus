# -*- coding: utf-8 -*-

from __future__ import absolute_import, division, print_function, unicode_literals

import nltk
from unittest import TestCase


class SentenceStemmer(object):
    # Example of Python-based string_types.

    @classmethod
    def create(cls, param):
        return cls()

    def __init__(self):
        self._stemmer = nltk.stem.PorterStemmer()

    def extract(self, text):
        tokens = nltk.wordpunct_tokenize(text)
        result = []
        for t in tokens:
            result.append((0, 0, self._stemmer.stem(t), 1.0))
        return result


class SentenceStemmerTest(TestCase):
    def test_extract(self):
        ex = SentenceStemmer.create({})
        sentence = '  The foxes JUMPED over dogs  '
        result = ex.extract(sentence)
        self.assertEqual(['the', 'fox', 'jump', 'over', 'dog'], [x[2] for x in result])
