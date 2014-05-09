======================
pfi::lang::noncopyable
======================

概要
====

コピー不可なクラスを作るシンプルな方法を提供する。

使い方
======

.. code-block:: c++

  class noncopyable

このクラスを、private継承してクラスを作る。

サンプルコード
==============

.. code-block:: c++

  class A : noncopyable{ // private継承させる
  };
  
  int main()
  {
    A a, b;
    a=b; // compile error!
  }
