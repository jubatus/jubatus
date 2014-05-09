==================
pfi::network::ipv4
==================

概要
====

IPv4のアドレスを表すクラス。

使い方
======

.. code-block:: c++

  ipv4_address::ipv4_address()
  ipv4_address::ipv4_address(unsigned char a, unsigned char b, unsigned char c, unsigned char d)

コンストラクタ

.. code-block:: c++

  ipv4_address::to_string()

文字列化

.. code-block:: c++

  const static ipv4_address::any
  const static ipv4_address::broadcast
  const static ipv4_address::loopback
  const static ipv4_address::none

定数なアドレスたち

その他、operator ==, !=, < などが定義されている。
