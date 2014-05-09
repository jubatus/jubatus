====================
pfi::network::socket
====================

概要
====

ストリームソケット(TCP/IPソケット)を扱うためのライブラリ。

大体の使い方
============

.. code-block:: c++

  stream_socekt sock;
  sock.connect("d.hatena.ne.jp", 80);
  sock.puts("GET /tanakh/ HTTP/1.0\r\n\r\n");
  sock.flush();
  string line;
  while(sock.getline(line)) cout<<line<<endl;

細かい使い方
============

.. code-block:: c++

  class stream_socket

TCP/IPのソケット。
入出力のバッファリング機能を有する。


.. code-block:: c++

  bool stream_socket::connect(const string &host, uint16_t port_num)

指定したホスト、ポートに接続する。
hostの解決はset_dns_resolver()で設定されたものを使う。
デフォルトではnormal_dns_resolverクラスが用いられる。

.. code-block:: c++

  bool stream_socket::shutdown()

接続を切る。

.. code-block:: c++

  bool stream_socket::is_connected()

接続されているかどうかを返す。

.. code-block:: c++

  const ipv4_address &stream_socket::remote_addr()

接続先のIPアドレスを取得する。

.. code-block:: c++

  uint16_t stream_socket::remote_port()

接続先のポート番号を取得する。

.. code-block:: c++

  int stream_socket::read(void *dat, int size)

sizeバイト読み込む。
返り値は実際に読み込んだバイト数。
sizeより小さい数が返るのは、途中でEOFになったときのみである。

.. code-block:: c++

  int stream_socket::write(const void *dat, int size)

sizeバイト書き込む。
返り値は実際に書き込んだバイト数。
sizeより小さい値が返るのは、途中で接続が切れたときのみである。

.. code-block:: c++

  int stream_socket::flush()

書き込みバッファをフラッシュする。
返り値はその時ソケットに書き込んだバイト数。
失敗したら負の値が返る。

.. code-block:: c++

  int stream_socket::getc()

一文字読み込む。
EOFに達していれば、EOFを返す。

.. code-block:: c++

  bool stream_socket::getline(string &str, int limit=-1)

一行読み込む。
最大でlimit文字読み込む。
limit<0の時は無制限。
失敗したらfalseを返す。

.. code-block:: c++

  bool stream_socket::puts(const string &str)

文字列を書き込む。
後ろに改行は付けない。
フラッシュもしない。
失敗したらfalseを返す。

.. code-block:: c++

  bool stream_socket::set_timeout(double sec)

読み書きのタイムアウトを設定する。

.. code-block:: c++

  bool stream_socket::set_recv_timeout(double sec)

読み込みのタイムアウトを設定する。

.. code-block:: c++

  bool stream_socket::set_send_timeout(double sec)

書き込みのタイムアウトを設定する。

.. code-block:: c++

  bool stream_socket::set_nodelay(bool on)

neagleアルゴリズムを無効にする(on=trueで無効)。
