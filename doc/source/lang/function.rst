===================
pfi::lang::function
===================

概要
====

いろいろな関数のような振る舞いをするものを一まとめに扱うためのクラス。

.. deprecated:: 4.0

  この機能はC++11からSTLで提供されるようになったため、
  後方互換性の維持のために残しているものの、 :code:`std::function` の利用を推奨する。

使い方
======

.. code-block:: c++

  int add(int a, int b){ return a+b; }
  function<int(int,int)> f=&add;
  cout<<f(1,2)<<endl; // 3

普通の関数も。

.. code-block:: c++

  class add{ public:
    int operator()(int a, int b) const{ return a+b; }
  };
  function<int(int,int)> f=add();
  cout<<f(1,2)<<endl; // 3

関数オブジェクトも。

.. code-block:: c++

  function<int(int)> f=bind1st(&add, 1);
  cout<<f(2)<<endl; // 3

stlが作った関数オブジェクトも。
