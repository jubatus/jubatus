============
インストール
============

必要なパッケージ
================

* Python (2.4 〜 3.2)
* gcc-c++ (3.4 〜 4.6)
* fcgi (オプショナル)
* mysql, postgresql (オプショナル)
* msgpack (オプショナル)
* Magick++ (オプショナル)
* gtest (オプショナル)

手順
====

::

  $ ./waf configure
  $ ./waf build
  $ sudo ./waf install

その他オプションは ./waf ----help を参照のこと。
