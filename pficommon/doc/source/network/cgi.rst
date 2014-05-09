=================
pfi::network::cgi
=================

概要
====

cgi/fcgi/http serverを楽に書くためのライブラリ。

使い方
======

rawテキストCGI
--------------

cgiクラスを継承してrunメソッドを実装して、
それをmainから呼び出す。

.. code-block:: c++

  class my_cgi : public cgi{
  public:
    void run(ostream &os){
      // implementation...
  
      head["Content-Type"]="text/plain";
      os<<"Hello, "<<query["name"]<<endl;
    }
  };
  
  int main(){
    run_cgi(my_cgi()).run();
  }

run()はosに文字列を出力する。
headでHTTPヘッダを設定できる。
デフォルト状態では、text/plainのUTF8になっている。
HTTPのクエリはqueryに入っている。

XHTML CGI
---------

XHTMLを生成したい時に便利なクラスである。
run()では、HTMLを生成するコードを書く。
このライブラリではHTMLは次のようなDSLで生成する。

.. code-block:: c++

  class my_cgi : public xhtml_cgi{
  public:
    void run(){
      html__{
        head__{
          title__{
            text__("タイトル");
          }
        }
        body__{
          a__{ href__ = "http://kzk9.net/blog/";
            text__("super blog!");
          }
          br__;
        }
      }
    }
  };

.. code-block:: c++

  tag__{
    ...
  }

これが、<tag>...</tag>というHTMLに相当する。
HTML4のタグは予め全部定義してある。
属性はタグのあとに書く。
文字列要素はtext__("hoge");
あるいは、prim__("hoge");
で注入できる。
textの文字列は自動的にHTMLエスケープされるが、
primはされない。

定義されていないタグ、属性が必要な場合は、

* tag__("tag-name")

* attr_("attr-name")

で任意の名前のタグ、属性が使える。

tag__の実体は例によってif文なので、
if文のブロックに使える構文は使える。

.. code-block:: c++

  head__ title__ text__ ("hoge");

や、
 
.. code-block:: c++

  hr__;

はvalidな構文です。

この様に書くと、裏でシコシコHTMLのツリーが組み立てられて、
run()の終了後それがXHTMLとして生成され、CGIの出力になる。

fcgiとして実行
--------------

fcgiとして実行するのに、my_cgiクラスを変更する必要はない。
単にrun_cgiの代わりにrun_fcgiを使うとfcgiとして実行されるようになる。

.. code-block:: c++

  int main(){
    run_fcgi(my_cgi()).run();
  }

なお、run_fcgiによって実行した場合でも、
プログラム自体をfcgiとして実行しなかった場合は
通常のcgiとして動作する。

スタンドアロンHTTPサーバとして実行
----------------------------------

run_cgi、run_fcgiの代わりにrun_serverを用いると、
スタンドアロンHTTPサーバとして実行される。

.. code-block:: c++

  int main(){
    run_server(my_cgi(), port_number /* = 8080 */, thread_num /* = 1 */, timeout /* = 10 */).run();
  }

コンストラクタで、ポート番号、スレッド数、通信タイムアウト(秒)が指定できる。省略すると適当な値が使われる。

.. code-block:: c++

  int main(int argc, char *argv[]){
    run_server(my_cgi(), argc, argv).run();
  }

argcとargvを渡すコンストラクタを用いると、コマンドライン引数でサーバの設定をすることができる。

.. code-block:: c++

  $ ./a.out --help
  usage: ./a.out [-p port] [-t thread-num] [-o timeout] [-h]

なお、スタンドアロンHTTPサーバを用いるときは、スレッドセーフのために、渡されたCGIオブジェクトをスレッド数だけクローンする。ゆえに、渡すCGIオブジェクトがclonableでなければならない。CGIオブジェクトは、デフォルトではclonableではない。

.. code-block:: c++

  class my_cgi : public xhtml_cgi, cloner<my_cgi> {
  public:
    void run(){
      html__{
        head__{
          ...

clonableにするには、cgiクラスをclonerと多重継承にする。clonerには実装するクラスの名前を渡す。clonerは、渡されたクラスをコピーコンストラクタでコピーする。clone()動作を変えるにはコピーコンストラクタを実装すれば良い。

run_serverはコンストラクタでソケットをlistenする。小さい番号のポートのためにrootでポートをlistenしてそれから降格してような場合は、次のように書ける。

.. code-block:: c++

  assert(getuid()==0);    // rootである
  my_cgi c;
  run_server serv(c, 80); // 80番ポートlisten
  setuid(...);            // 降格
  serv.run();             // サーバ処理開始
 
きめ細かい制御のために、listenするソケットを渡すこともできる。

.. code-block:: c++

  run_server::socket_type ssock(new server_socket()); // ソケットを自分で作成
  ssock->create(8080);
  ssock->set_timeout(10);
  run_server(my_cgi(), ssock).run(); // run_serverに渡す

run_server::socket_typeは、server_socketへのスマートポインタの型である（型名が長いのでtypedefしている）。クライアントとの接続のタイムアウトは、サーバーソケットの設定が継承される。

run_server::run()にfalseを渡すことにより、非同期実行が可能である。次の例は8080、8081の二つのポートでサーバを立てる。

.. code-block:: c++

  int main(){
    my_cgi m;
    run_server serv1(m, 8080);
    run_server serv2(m, 8081);
    serv1.run(false); // サーバを立てた後帰ってくる
    serv2.run(false); // サーバを立てた後帰ってくる
  }

非同期にサーバを立てた後、run_server::join()を使って明示的に終了を待つことができる(サーバは終了しないので、永遠に待つことになる)。明示的にjoin()しない場合でも、デストラクタでjoin()する。なので、上のコードは正しく動作する。

なお、デストラクタでjoin()するので、

.. code-block:: c++

  int main(){
    run_server(my_cgi()).run(true);
  }

と、


.. code-block:: c++

  int main(){
    run_server(my_cgi()).run(false);
  }

は同じ動作になる。
