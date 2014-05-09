==================
pfi::network::http
==================

概要
====

HTTPを扱うライブラリ。

使い方
======

リクエスト、レスポンスを直接扱う、ローレベルなライブラリと、
ストリーム形式のハイレベルなライブラリが用意されている。

ローレベルライブラリ
====================

requestクラス
-------------

.. code-block:: c++

  request::request(const string &method, const uri &u, int major_ver=1, int minor_ver=1)

requestを作る。
メソッド、URI、HTTPのバージョンが指定できる。

.. code-block:: c++

  request::request(shared_ptr<stream_socekt> sock)

ソケットからリクエストを読む。

.. code-block:: c++

  const string &request::method() const
  const uri &request::path() const
  const pair<int,int> &request::version() const

アクセッサ

.. code-block:: c++

  header &head()

ヘッダ用アクセッサ。
設定したり取得したり。

.. code-block:: c++

  iostream &body()

bodyを表すiostreamを返す。
送るデータを入れたり、
送られたデータを取り出したり。

自分で作ったrequestクラスのbodyの実体は、stringstreamである。
メモリ上に一旦貯めるので、大きなリクエストを送信する時は注意すること。
bodyに何も書き込まずにsend()した場合、bodyの実態がソケットになるので、
大きなリクエストの転送にも利用できる。
しかし、この場合リクエストを再利用することができなくなる。
一度send()したら破棄しなければならない。
socketから読み込んだrequestの場合はソケットになるので、
メモリは消費しない。

.. code-block:: c++

  request::send(shared_ptr<stream_socket> sock)

sockにHTTPリクエストを送信する。

responseクラス
--------------

.. code-block:: c++

  response::response(int code, const string &reason, int major_ver=1, int minor_ver=1)

レスポンスクラスを作る。

.. code-block:: c++

  response::response(shared_ptr<stream_socket> sock)

ソケットからレスポンスを読み込む。

.. code-block:: c++

  const pair<int,int> &response::version() const
  int response::code() cosnt
  const string &response::reason() const

アクセッサ

.. code-block:: c++

  header &response::head()

ヘッダ

.. code-block:: c++

  iostream &response::body()

bodyを表すiostream。
実体は、大体requestのそれと同じ。

.. code-block:: c++

  void response::send(shared_ptr<stream_socket>)

ソケットにresponseの内容を送信。

headerクラス
------------

HTTPのヘッダを表すクラス。

.. code-block:: c++

  string &header::operator[](const string &key)

ヘッダの要素にアクセス。
keyの大文字と小文字は区別しないが、
挿入した際の大文字・小文字は保存される。

.. code-block:: c++

  hd["Content-Type"]="hoge";
  hd["content-type"]="moge";

と書くと、

.. code-block:: c++

  Content-Type: moge

というヘッダが送信される。
取得する時も同様に、

.. code-block:: c++

  resp.head()["Content-Type"]

とアクセスすると、実際に受信したContent-Typeの大文字小文字によらず
要素が取得できる。

.. code-block:: c++

  iterator header::begin()
  iterator header::end()

要素のイテレートもできる。
iteratorはpair<string,string>を要素としてもつものとしてアクセスできる。

.. code-block:: c++

  bool header::has_key(const string &key) const

ヘッダがkeyを持っているかどうか、内容を変更せずにチェックできる。
keyの大文字小文字は無視する。

サンプルコード
--------------

.. code-block:: c++

  #include <iostream>
  using namespace std;
  
  #include <pficommon/lang/shared_ptr.h>
  #include <pficommon/network/socket.h>
  #include <pficommon/network/http.h>
  using namespace pfi::lang;
  using namespace pfi::network;
  using namespace pfi::network::http;
  
  int main()
  {
    shared_ptr<stream_socket> ss(new stream_socket());
    request req("GET", uri("http://kzk9.net/blog/"), 1, 1);
    req.head()["Host"]="kzk9.net";
    ss->connect("kzk9.net", 80);
    req.send(ss);
    response resp(ss);
  
    for (string line; getline(resp.body(), line); )
      cout<<line<<endl;
  
    return 0;
  }

ハイレベルライブラリ
====================

httpstreamというistream互換のクラスが用意されている。
これを用いるとGETリクエストを簡単に送ることができる。

なおこのクラスは、失敗したときに pfi::network::http::http_exception を投げる。

サンプルコード
--------------

.. code-block:: c++

  httpstream hs("http://kzk9.net/blog/");
  for (string line; getline(hs, line); )
    cout<<line<<endl;
