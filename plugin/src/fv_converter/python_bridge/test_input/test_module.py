# -*- coding: utf-8 -*-

from __future__ import absolute_import, division, print_function, unicode_literals


unicode_t = type(u'')
bytes_t = type(b'')


class Base(object):
    @classmethod
    def create(cls, param):
        return cls()


class StringFeature(Base):
    def extract(self, text):
        assert type(text) == unicode_t
        return [(0, 0, 'test', 1.0)]


class NumFeature(Base):
    def extract(self, key, value):
        assert type(key) == unicode_t
        assert type(value) == float
        return [('result', 1.0)]


class BinaryFeature(Base):
    def extract(self, key, value):
        assert type(key) == unicode_t
        assert type(value) == bytes_t
        return [('result', 1.0)]


class WordSplitter(Base):
    def split(self, text):
        assert type(text) == unicode_t
        return [(0, 1)]
