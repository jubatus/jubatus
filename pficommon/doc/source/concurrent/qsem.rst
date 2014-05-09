=====================
pfi::concurrent::qsem
=====================

概要
====

Quantity Semaphore

使い方
======

.. code-block:: c++

  qsem::qsem(int q)

量を指定して初期化。

.. code-block:: c++

  qsem::wait(int q=1)
  qsem::signel(int q=1)

ロック・アンロック。
量が指定できる。

.. code-block:: c++

 qsem::lock()
 qsem::unlock()

qsem::wait(1), qsem::signel(1) と同じ。
lockableインターフェースの実装である。
