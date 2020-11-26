===============
pfi::lang::ref
===============

概要
====

参照をラップする :code:`reference_wrapper` 、
非const参照の変数をラップして :code:`reference_wrapper` オブジェクトを生成する :code:`ref` と
const参照をラップして同オブジェクトを生成する :code:`cref` を提供する。

.. deprecated:: 4.0

  これらの機能はC++11からSTLで提供されるようになったため、
  後方互換性の維持のために残しているものの、 :code:`std::reference_wrapper` :code:`std::ref` :code:`std::cref` の利用を推奨する。
