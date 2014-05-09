=================
pfi::lang::mem_fn
=================

概要
====

stlには、メンバ関数を関数化するための
mem_fun、mem_fun_ref、mem_fun1、mem_fun1_ref
さらにそれぞれのconst版、
const_mem_fun、const_mem_fun_ref、const_mem_fun1、const_mem_fun1_ref
があるが、煩雑極まりないのと、2引数以上のメンバ関数を扱えるようにするために、
これをまとめて、さらに一般化したものがmem_fnである。

使い方
======

.. code-block:: c++

  class foo{
  public:
    int hoge(int n){
      return n*2;
    }
  
    int moge(int n) const{
      return n*3;
    }
  };

このようなクラスがあったとき、

.. code-block:: c++

  foo a;
  
  function<int(foo&,int)> ff=mem_fn(&foo::hoge);
  cout<<ff(a,444)<<endl;
  
  function<int(const foo*,int)> fg=mem_fn(&foo::moge);
  cout<<fg(&a,444)<<endl;

このようにメンバ関数を関数オブジェクト化することができる。
