==========================
pfi::concurrent::condition
==========================

概要
====

条件変数を扱うためのクラス

使い方
======

mutexにロックをかけcondition::waitにそれを渡せば、
そのconditionがsignalされるまで待つことができる。

.. code-block:: c++

  mutex m;
  condition c;
  
  synchronized(m){
    if (!hoge) c.wait(m); // cで待つ
    ...
    c.notify_all(); // 他の待ってる人を起こす
  }

時限付きwaitもできる。

.. code-block:: c++

  if (!c.wait(m, 10)) // 10秒だけ待ってみる
