============
インストール
============

必要なパッケージ
================

* Python (2.5 〜 3.5)

  + 3.6, 3.7 は現在テスト中。

* gcc-c++ (3.4 〜 5.5)
* bzip2
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
