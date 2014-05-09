===============
pfi::lang::cast
===============

概要
====

C++的なキャストを用意。

lexical_cast
============

.. code-block:: c++

  template <class T, class U>
  T lexical_cast(const U &r);

T型への字句的キャスト。
実装は、stringstreamに突っ込んでstringstreamから取り出しているだけ。

.. code-block:: c++

  string num("123");
  cout<<lexical_cast<int>(num)*2<<endl; // 246
