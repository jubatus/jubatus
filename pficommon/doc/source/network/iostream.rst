======================
pfi::network::iostream
======================

概要
====

TCPソケットに対するstreamの実装。

大まかな使い方
==============

コンストラクタにホスト名を指定し、
接続に成功した場合には、通常のiostreamとして利用できる。

.. code-block:: c++

  socketstream ss("d.hatena.ne.jp");
  if (!ss){ ... } // つながらなかった
  ss<<"GET /kzk/ HTTP/1.0\r\n\r\n"<<flush;
  for (string line; getline(ss, line); )
    cout<<line<<endl;
