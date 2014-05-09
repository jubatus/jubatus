======================
pfi::concurrent::pcbuf
======================

概要
====

Producer-Consumer Buffer。作成時に容量の上限の指定ができ、タイムアウト付きのpush/popができるのが特徴である。

使い方
======

.. code-block:: c++

  explicit pcbuf<T>::pcbuf(size_t capacity)

容量の上限を指定して空のキューを作る。

.. code-block:: c++

  void pcbuf<T>::push(const T& value)

キューにvalueを追加する。
キューが満杯なら空きができるまで待つ。

.. code-block:: c++

  bool pcbuf<T>::push(const T& value, double second)

キューにvalueを追加する。
キューが満杯のときは、タイムアウト付き(タイムアウト時間はsecond秒)で空きができるのを待つ。
キューに要素を追加できたときはtrueを、そうでない(タイムアウト)ならfalseを返す。

.. code-block:: c++

  void pcbuf<T>::pop(T& value)

キューから取り出してvalueに入れる。
キューが空なら追加されるまで待つ。

.. code-block:: c++

  bool pcbuf<T>::pop(T& value, double second)

キューから取り出してvalueに入れる。
キューが空のときは、タイムアウト付き(タイムアウト時間はsecond秒)で要素が追加されるのを待つ。
キューから要素を取り出せたときはtrueを、そうでない(タイムアウト)ならfalseを返す。

.. code-block:: c++

  void pcbuf<T>::clear()

キューを空にする。

.. code-block:: c++

  size_t pcbuf<T>::size() const

キューに格納している要素の数を返す。

.. code-block:: c++

  size_t pcbuf<T>::capacity() const

キューに格納できる要素の最大数を返す。

.. code-block:: c++

  bool pcbuf<T>::empty() const

キューに格納している要素の数が0であるならtrueを、そうでないないならfalseを返す。

サンプルコード
==============

.. code-block:: c++

  pcbuf<int> q(7); // int型を7個まで格納できるキューを作る
  
  q.push(2); // 2をキューに追加する
  
  int v;
  q.pop(v); // 2をキューから取り出してvに入れる。
  
  if (!q.push(3, 0.1)) { // 3をキューに追加する(タイムアウトは0.1秒に設定)
    cerr << "push timeout" << endl;
  }
  
  if (!q.pop(v, 0.5)) { // 3をキューから取り出してvに入れる(タイムアウトは0.5秒に設定)
    cerr << "pop timeout" << endl;
  }
