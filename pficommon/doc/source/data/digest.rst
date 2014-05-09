=================
pfi::data::digest
=================

概要
====

データダイジェスト。
現在のところ、MD5の実装がある。

使い方
======

.. code-block:: c++

  template <class Iterator>
  md5sum::md5sum(Iterator begin, Iterator end)

charを返す入力イテレータからmd5sumを計算

.. code-block:: c++

  md5sum::md5sum(std::istream &is)

istreamからmd5sumを計算

.. code-block:: c++

  md5sum::md5sum(const std::string &str)

std::stringからmd5sumを計算

サンプルコード
==============

.. code-block:: c++

  md5sum m("hoge hoge hoge!");
  cout<<m<<endl; // md5を16進で出力
  cout<<m.a<<" "<<m.b<<" "<<m.c<<" "<<m.d<<endl; // 32ビットごとに取り出せる
  cout<<m<<endl; // 16進32桁で表示
