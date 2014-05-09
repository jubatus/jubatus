=====================
pfi::concurrent::mvar
=====================

概要
====

同期変数

使い方
======

.. code-block:: c++

  mvar<T>::mvar()

空のmvarを生成

.. code-block:: c++

  mvar<T>::mvar(const T &v)

vが入ったmvarを生成

.. code-block:: c++

  T mvar<T>::take()

mvarの中身を取り出す。
中身がなければ誰かが入れるまで待つ。

.. code-block:: c++

  void mvar<T>::put(const T &v)

mvarにvを入れる。
すでに中身が入っていれば、誰かが取り出すまで待つ。

.. code-block:: c++

  T mvar<T>::read()

mvarの中身を見る。
中身が無ければ誰かが入れるまで待つ。

.. code-block:: c++

  bool mvar<T>::try_take(T &ret)

mvarの中身を取り出してみる。
中身が無ければfalseを返す。
あればtakeしてtrueを返す。

.. code-block:: c++

  bool mvar<T>::try_put(const T &v)

mvarにvを入れようとしてみる。
中身が無ければputしてtrueを返す。
あれば、falseを返す。

.. code-block:: c++

  bool mvar<T>::empty()

中身が空かどうかを返す。

サンプルコード
==============

.. code-block:: c++

  mvar<string> mv;
  
  void foo()
  {
    for (;;){
      string str;
      if (mv.try_take(str)){
        if (str=="quit")
          break;
        cout<<"*** "<<str<<endl;
      }
      thread::sleep(5);
    }
  }
  
  int main()
  {
    thread thr(&foo);
    thr.start();
  
    for (;;){
      string str;
      cout<<"> "<<flush;
      getline(cin,str);
  
      mv.put(str);
  
      if (str=="quit")
        break;
    }
    thr.join();
    
    return 0;
  }
