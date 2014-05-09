=====================
pfi::concurrent::lock
=====================

概要
====

マルチスレッドで同期を取るためのライブラリ

使い方
======

lockaleなクラス
---------------

lock() と unlock() ができるクラスである。
次のクラスが用意されている。

* mutex
  普通のミューテックス。

* r_mutex
  再帰的にロックができるミューテックス。

* rw_mutex
  read/write ロックができるミューテックス。さらに制限時間付きロックもできる。
  read_lock(), write_lock()で呼び分ける。
  unlock()はどちらでも同じ。
  lock()はread_lock()を呼びだす。

scoped_lock
-----------

lockableなクラスを受け取るRAIIクラス。
コンストラクタでロックをかけて、
デストラクタでアンロックするクラスである。

.. code-block:: c++

  mutex m;
  {
    scoped_lock lk(m);
    ... 
  }

便利な書き方
------------

* synchronized

java風にブロックロックが書ける。

.. code-block:: c++

  mutex m;
  synchronized(m){
    // ここの範囲でロック
  }

* read/write locker

rw_mutexのロックを便利にする。

.. code-block:: c++

  rw_mutex m;
  {
    scoped_lock lk(rlock(m));
    // read lock
  }

.. code-block:: c++

  rw_mutex m;
  {
    scoped_lock lk(wlock(m));
    // write lock
  }

.. code-block:: c++

  rw_mutex m;
  {
    scoped_lock lk(rlock(m, 10));
    // 制限時間付き read lock
  }
 
.. code-block:: c++

  rw_mutex m;
  {
    scoped_lock lk(wlock(m, 10));
    // 制限時間付き write lock
  }

もちろんsynchronizedにも渡せる。

.. code-block:: c++

  rw_mutex m;
  synchronized(rlock(m)){
  }

synchronizedの実体はifなので、
ロックがかからなかった時の処理を書くことができる。

.. code-block:: c++

  rw_mutex m;
  synchronized(rlock(m, 5)){
  }
  else{
   //
  }

elseを書かない場合は、予期せぬインデントに注意すべし。
