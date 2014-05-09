==============
pfi::math::fft
==============

概要
====

高速離散フーリエ変換のためのライブラリ。

使い方
======

.. code-block:: c++

  fft(Iterator begin, Iterator end)

complex<浮動小数>なコンテナに対するランダムアクセスイテレータを渡すとそこをFFTする。
現在のところ長さが2^kでないとエラーになる。

.. code-block:: c++

  ifft(Iterator begin, Iterator end)

フーリエ逆変換を行います。入力の仕様はfft()と同じである。

.. code-block:: c++

  fft2d(Iterator begin, Iterator end, size_t pitch)

二次元FFTを行う。
データはリニアに格納されているとし、幅をpitchで与える。
入力に与えられる長さはpitchの倍数でなくてはならず、
幅と高さがそれぞれ2^kである必要がある。

.. code-block:: c++

  template <class Matrix>
  fft2d(Matrix &m)

二次元FFTを行う。
m[0][0]なる方法でアクセスできる二次元配列を渡す。
それぞれの要素の長さはすべて同じである必要がある。
長さに関する制約はリニア版と同じである。

.. code-block:: c++

  ifft2d(Iterator begin, Iterator end, size_t pitch)
  template <class Matrix>
  ifft2d(Matrix &m)

二次元フーリエ逆変換を行う。
入力に関する制約はfft2dと同じである。
