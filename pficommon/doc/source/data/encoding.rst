===================
pfi::data::encoding
===================

概要
====

いろいろなエンコード。
現在のところbase64の実装がある。

使い方
======

.. code-block:: c++

  std::string base64_encode(const std::string &str)

与えられた文字列をbase64エンコードした文字列を返す。

.. code-block:: c++

  std::string base64_decode(const std::string &base64str)

与えられたbase64文字列をデコードした文字列を返す。
与えられた文字列がbase64として正しくなかった場合、
invalid_argumen例外が投げられる。

サンプルコード
==============

.. code-block:: c++

  string s = base64_encode("hoge");
  string t = base64_decode(s);
  assert(s == t);
