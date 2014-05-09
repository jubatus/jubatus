====================
pfi::lang::safe_bool
====================

概要
====

boolとして評価したいクラスを作りたいときに使う。

使い方
======

.. code-block:: c++

  class A : public safe_bool<A>{
  public:
    bool bool_test() const { return ...; }
  };

safe_bool<A>から"public"継承させる。
safe_boolのパラメータには自分を渡す
(このような手法はCRTPなどと呼ばれる)。

それから、publicメンバ関数として、bool bool_test() constな関数を実装する。
これでAをboolとして評価できるようになる。

.. code-block:: c++

  A a, b;
  if (a){ ... } // ok!
  if (a<b){ ... } // compile error!
  hoge(a); // compile error!

operator bool() では駄目な理由
==============================

キャスト演算子を定義すると、その型への暗黙の変換が生じる。

.. code-block:: c++

  class A{ public: operator bool() const { return ... } };

このようなクラスがあると、

.. code-block:: c++

  A a, b;
  if (a<b){ // compile OK!
    ...

この様なコードがコンパイルを通ってしまう。
boolへの変換を定義しても、
この様なコードが通るのはプログラマの意図に反するものである可能性が高い。
また、intを受け取る関数にも渡せるようになってしまう。

.. code-block:: c++

  int hoge(int a){ ... }
  
  int main(){
    A a;
    cout<<hoge(a)<<endl; // compile OK!
    ...

safe_boolを用いると、
このような意図しない暗黙の変換によって予期せぬコードが
コンパイルを通ってしまう状況を防ぐことができる。
