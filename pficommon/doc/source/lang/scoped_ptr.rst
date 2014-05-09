=====================
pfi::lang::scoped_ptr
=====================

概要
====

コピーできないauto_ptrのようなもの。

shared_ptrのコピーセマンティクスが必要ない場面ではscoped_ptrを使うべきである。
scoped_ptrは空間のオーバーヘッドは0で利用でき、
インライン化によって時間のオーバーヘッドも恐らくない。
shared_ptrは、ポインタ一つにつきポインタ一つ分のオーバーヘッド、
さらに、非参照オブジェクト1つにつきポインタ一つ分のオーバーヘッドがかかる。

使い方
======

変数として使ったり、

.. code-block:: c++

  void foo(){
    scoped_ptr<string> p(new string("hoge"));
    ...
    // 解放される
  }

クラスのメンバとして使ったり

.. code-block:: c++

  class Hoge{
  public:
     Hoge(): p(new string("hoge")) {}
  private:
     scoped_ptr<string> p;
  };

参照を取得

.. code-block:: c++

  *p

ポインタを取得

.. code-block:: c++

  p.get()

ポインタ風にメンバにアクセス

.. code-block:: c++

  p->hoge

boolへの暗黙の変換は持たない。

.. code-block:: c++

  if (!p) // compile error!

というのも、scoped_ptrにNULLを入れることは許されていないので、
この式が意味を持たないからである。
