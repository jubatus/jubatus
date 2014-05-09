=================
pfi::network::rpc
=================

概要
====

C++間で簡単にRPCをするためのライブラリ。

使い方
======

シグニチャの定義
----------------

RPC_PROCというマクロで関数を定義して、
RPC_GENというマクロでサーバとクライアントのクラスを生成する。

.. code-block:: c++

  RPC_PROC(<関数名>, <シグニチャ>)

二つ整数をとって整数を返す、足し算のような関数を定義するなら、

.. code-block:: c++

  RPC_PROC(add, int(int,int))

のようになる。

.. code-block:: c++

  RPC_GEN(<バージョン>, <プリフィックス>, <関数>, ...)

バージョンには適当な整数を指定する
(サーバとクライアントでこの数値が一致しないとRPC呼び出しは失敗する)。
プリフィックスには、サーバとクライアントのクラス名のプリフィックスを指定する。
hoge と指定すると、hoge_server、hoge_clientがそれぞれサーバとクライアントのクラス名になる。
3引数目以降には関数のリストを指定する。

.. code-block:: c++

  RPC_GEN(1, hoge, add, mul)

などが記述の一例である。

サーバのコード
--------------

hoge_serverクラスのインスタンスを生成し、
set_xxx()関数によって、実際に処理をさせる関数をセットし、
serv()関数でサーバを開始する。

.. code-block:: c++

  int add(int x, int y){ return x+y; }
  
  int main(){
    hoge_server serv;
    serv.set_add(&add);
    serv.serv(12345, 10); // ポート番号, スレッド数
  }

クライアントのコード
--------------------

hoge_clientクラスのインスタンスを生成し、
call_xxx()で呼び出せる。

.. code-block:: c++

  hoge_client cli("localhost", 12345); // ホスト、ポート番号
  cout<<cli.call_add(1,2)<<endl; // 3が返ってくるはず

呼び出しに失敗したりすると、call_xxx()はpfi::network::rpc::rpc_errorもしくはそれを継承した例外を投げる。

* cannot_get_connection 接続できなかった
* method_not_found サーバに要求したメソッドがなかった
* version_mismatch クライアントの要求したバージョンがサーバのバージョンと異なった

ストリームの受け渡し
--------------------

大きなファイルをメモリを使わずに転送したい場合などに、
ストリーム経由での引数の受け渡しが使える。

.. code-block:: c++

  RPC_PROC(foo, void(pfi::data::serialization::stream))

と、引数をpfi::data::serialization::streamにすると、
クライアント側は、

.. code-block:: c++

  fstream fs("hoge.txt", ios::in);
  cli.call_foo(stream(f));

のように、iostream(istreamやostreamではだめなので注意)をstreamでくるんで送ることができる。

サーバ側は

.. code-block:: c++

  void foo(stream s)
  {
  }

の s に適当なiostreamが作られて呼び出される
(具体的には/tmp/以下に受信したデータを含むファイルが作られ、それがopenされ、sにbindされて呼び出される)。

.. code-block:: c++

  void foo(stream s)
  {
    istream &is=s.get();
    // isをつかうコード
  }

fooの実行が終わると、テンポラリファイルは自動的に削除される。

注意
====

RPCの引数、返り値はpficommonのシリアライズライブラリによって
シリアライズ可能な型である必要がある。
シリアライザに関しての詳細はそちらを参照のこと。
ちなみに、C++のプリミティブな型や、
stlのiteratableなコンテナはすべてデフォルトでシリアライズ可能である。

RPCの引数はread-onlyである。
参照を渡してwrite-backなどということはできない。
そもそもシグニチャには参照を書いてはいけない。
複雑なデータを返したいときは、
シリアライズ可能な構造体を作って、それで返すようにする。
