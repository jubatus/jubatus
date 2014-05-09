========================
pfi::data::serialization
========================

概要
====

C++のデータ型をバイト列にシリアライズしたりデシリアライズするためのライブラリ。

使い方
======

基本的にBoostのSerializationの模倣である。

データに対して、シリアライズ関数を定義して、
ストリームを用意して、
アーカイバにくっつけて、
アーカイバにデータを送ることができる。

シリアライズ
------------

.. code-block:: c++

  binary_oarchive oa(cout); // ostreamを指定してbinary_oarchiveを作る
  int a;
  string s;
  oa << a << s; // ostream的な

デシリアライズ
--------------

.. code-block:: c++

  binary_iarchive ia(cin); // istreamを指定してbinary_iarchiveを作る
  int a;
  string s;
  ia >> a >> s; // istream的な

ストリームが指定できるので、socketstream等を指定するとネットワーク越しに
やりとりすることができる。

シリアライズできるデータ
------------------------

STLのiteratableなコンテナはすべてデフォルトでサポートしている。
ユーザ定義型のシリアライズは次項。

ユーザ定義型のシリアライズ(侵入型)
----------------------------------

.. code-block:: c++

  class hoge{
  public:
    hoge(){}
  private:
    friend class pfi::data::serialization::access;
    template <class Ar>
    void serialize(Ar &ar){
      ar & a & s;
    }
    int a;
    string s;
  };

template <class Ar> void serialize(Ar &ar) なるメソッドを実装すれば、
アーカイバに投入できるようになる。

serializeメソッドはprivateにすること。
その際、シリアライザがserializeメソッドにアクセスできるようにするために、

.. code-block:: c++

  friend class pfi::data::serialization::access;

にてfriend classを定義する。

ユーザ定義型のシリアライズ(非侵入型)
------------------------------------

.. code-block:: c++

  class hoge{
  public:
    hoge(){}
 
    int &get_a(){ return a; }
     string &get_s(){ return s; }
 
  private:
    int a;
    string s;
  };
 
  template <class Ar>
  void serialize(Ar &ar, hoge &h){
    ar & h.get_a() & h.get_s();
  }

serialize(Ar &ar, T &v); なるグローバル関数を定義することによっても
型Tがシリアライズ可能になる。
内部の変数が外からアクセスできる場合はこのように書くことができる。
他人の書いたクラスのシリアライズ等に利用できる。
詳しくは、pficommonのSTLのコンテナのシリアライズのコードを参照のこと。

シリアライズとデシリアライズでコードを分けたい
----------------------------------------------

.. code-block:: c++

  template <class Ar>
  void serialize(Ar &ar, T &v){
    if (ar.is_read){
      // デシリアライズ
    }
    else{
      // シリアライズ
    }
  }

いろいろの簡便のため、処理速度を犠牲にしてシンプルに書けるようにした。
Boostとは違うので注意。
