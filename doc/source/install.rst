============
インストール
============

必要なパッケージ
================

* Python (3.6 - 3.7)
* gcc-c++ (4.8.5 - 9.3.0)
* bzip2
* fcgi (オプショナル)
* mysql, postgresql (オプショナル)
* msgpack (オプショナル)
* gtest (オプショナル)

手順
====

.. code-block:: console

  $ ./waf configure
  $ ./waf build
  $ sudo ./waf install

その他オプションは ``./waf --help`` を参照のこと。
