=================
pfi::math::random
=================

概要
====

乱数を生成するためのライブラリ。

使い方
======

.. code-block:: c++

  class random<Gen>

ランダムクラス。Genに乱数生成器を指定する。

.. code-block:: c++

  random::random()

デフォルトコンストラクタ。
時刻をseedに乱数生成器を初期化。

.. code-block:: c++

  random::random(uint32_t seed)

seedを指定して初期化。

.. code-block:: c++

  uint32_t random::next_int()

[0,0xffffffff]の範囲の整数を生成。

.. code-block:: c++

  uint32_t random::next_int(uint32_t a)

[0,a)の範囲の整数を生成。

.. code-block:: c++

  uint32_t random::next_int(uint32_t a, uint b)

[a,b)の範囲の整数を生成。

.. code-block:: c++

  double next_double()

[0,1)の範囲の浮動小数を生成。
53ビットの解像度がある。

.. code-block:: c++

  double next_gaussian()

平均0、分散1の正規分布で浮動小数を生成。

.. code-block:: c++

  double next_gaussian(double mean, double deviation)

平均と分散を指定して生成。

.. code-block:: c++

  uint32_t operator()()
  uint32_t operator(uint32_t a)()
  uint32_t operator(uint32_t a, uint32_t b)()

next_int()のそれぞれに対応。
random_shuffle()などに使いやすいように。

ジェネレータ
------------

.. code-block:: c++

  gen::gen(uint32_t seed)
  uint32_t gen::next()

uint32_tのseedを引数にとり、
uint32_tで範囲[0,0xffffffff]の乱数を返すクラス。

メルセンヌツイスタ実装が
pfi::math::random::mersenne_twisterに用意されている。
さらに

.. code-block:: c++

  typedef random<mersenne_twister> mtrand;

が定義されているので、手軽に使える。

サンプルコード
==============

.. code-block:: c++

  int main()
  {
    mtrand r(777);
  
    cout<<r.next_int()<<endl;
    cout<<r.next_double()<<endl;
  
    int tbl[10]={0,1,2,3,4,5,6,7,8,9};
    random_shuffle(tbl, tbl+10, r); // こういうのに渡せる
  
    return 0;
  }
