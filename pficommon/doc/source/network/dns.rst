=================
pfi::network::dns
=================

概要
====

DNSリゾルバ。

使い方
======

dns_resolverクラス
------------------

DNS解決をするクラス。

.. code-block:: c++

  vector<ipv4_address> dns_resolver::resolve(const string &host, uint16_t port)

dns_resolverが持つ唯一のメソッド。
DNSの解決をする。返り値はIPV4アドレスのリスト。

normal_dns_resolverクラス
-------------------------

毎回問い合わせる普通のリゾルバ。
socket_streamクラスのデフォルトのリゾルバである。

cached_dns_resolverクラス
-------------------------

キャッシュ付きリゾルバ。

.. code-block:: c++

  cached_dns_resolver::cached_dns_resolver(int max_size, int expire_second)

保持する最大のエントリ数と、保持する最長の時間を指定して
キャッシュ付きリゾルバを作る。

.. code-block:: c++

  cached_dns_resolver::clear_cache()

キャッシュしているものを全部捨てる。

.. code-block:: c++

  cached_dns_resolver::delete_cache(const strig &host, uint16_t port)

特定のエントリのキャッシュを消去する。
