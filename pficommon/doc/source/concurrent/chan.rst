=====================
pfi::concurrent::chan
=====================

概要
====

同期多重化キュー

使い方
======

.. code-block:: c++

  chan<T>::chan()

空のキューを作る。

.. code-block:: c++

  chan<T>::write(const T &r)

キューにrを入れる。

.. code-block:: c++

  T chan<T>::read()

キューから取り出す。
キューが空なら待つ。

.. code-block:: c++

  T chan<T>::unget(const T&r)

キューの先頭にrを戻す。

.. code-block:: c++

  bool chan<T>::empty()

キューが空かどうかを返す。

.. code-block:: c++

  shared_ptr<chan<T> > chan<T>::dup()

キューを複製する。
複製されたキューは多重化され、
どちらのキューにwriteされたデータも、
両方のキューからreadできる
（ungetは多重化されない）。
