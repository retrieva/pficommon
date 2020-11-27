====================
pfi::lang::enable_if
====================

概要
====

.. code-block:: c++

  template <bool B_, class T = void>
  using enable_if = std::enable_if<B_, T>;

  template <bool B_, class T = void>
  struct disable_if : enable_if<!B_, T> {};

コンパイル時、 :code:`B_` がtrueであれば :code:`type` メンバを持つ :code:`enable_if` 、 falseであれば :code:`type` メンバを持つ :code:`disable_if` を提供する。


使い方
======

.. code-block:: c++

  template <class T>
  auto f(T) -> typename pfi::lang::enable_if<std::is_integral<T>::value>::type {}

関数の引数や戻り値、テンプレート引数のデフォルト値などで :code:`enable_if` や :code:`diable_if` の :code:`type` メンバを使用すると、 SFINAEにより :code:`type` メンバが存在しない場合はその関数はオーバーロード解決の対象から除外される。


サンプルコード
=============

.. code-block:: c++

  template <class T>
  auto f(T) -> typename pfi::lang::enable_if<std::is_integral<T>::value>::type
  {
    std::cout << "Tは整数型" << std::endl;
  }

  template <class T>
  auto f(T) -> typename pfi::lang::disable_if<std::is_integral<T>::value>::type
  {
    std::cout << "Tは整数型以外" << std::endl;
  }

  f(3);  // Tは整数型
  f("hello");  // Tは整数型以外

