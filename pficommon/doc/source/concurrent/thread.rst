=======================
pfi::concurrent::thread
=======================

概要
====

スレッドを操作するためのクラス。

メンバ
======

.. code-block:: c++

  thread::thread(function<void()>);

function<void()>にコンパチブルな関数等をコンストラクタに渡す。
この時点ではまだスレッドは起動しない。

.. code-block:: c++

  thread::~thread()

detachしておらず、さらにまだスレッドが動いている場合、
スレッドをdetachする。

.. code-block:: c++

  bool thread::start()

startメソッドを呼べばスレッドが起動する。
失敗するとfalseを返す。

.. code-block:: c++

  bool thread::join()

スレッドが終わるのを待つ。
start()してない、detach()済みの場合、あるいは失敗した時はfalseを返す。

.. code-block:: c++

  void thread::detach()

スレッドをdetachする。
detachされたスレッドはjoinする必要がなくなる(joinできなくなる)。

staticメンバ
------------

.. code-block:: c++

  static tid_t thread::id()

呼び出したスレッドのスレッドIDを返す。

.. code-block:: c++

  static bool sleep(double sec);

指定した秒数、自分のスレッドの実行を休止する。

.. code-block:: c++

  static void yield();

他のスレッドに実行を明け渡す。

サンプルコード
==============

.. code-block:: c++

  void foo()
  {
    for (int i=0;i<10000;i++) cout<<"moge"<<endl;
  }
  
  int main()
  {
    thread t(&foo);
    if (!t.start()){ // スレッドを立てる
      // 立たなかった
      ...
    }
    t.join(); // スレッドを待つ
   }

detachスレッドを作りたい場合、次のようにするのが簡単である。

.. code-block:: c++

  thread(&foo).start();
