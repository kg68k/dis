# コンパイル方法

## Linux

srcディレクトリでmakeを実行します。

## Windows: Visual Studio 2022

プロジェクトファイルは用意しておりませんが、下記設定でビルドできます。

1. 新規プロジェクトにsrcディレクトリのファイルを追加します。
2. プロパティ &gt; C/C++ &gt; 詳細設定 &gt;「指定の警告を無効にする」に`4996;33010;33011`を追加する。
3. プロパティ &gt; C/C++ &gt; コマンドライン &gt;「追加のオプション」に`/source-charset:utf-8 /J`
   を追加する。


## 未確認環境

下記の環境でのコンパイルは確認していません。

* FreeBSD -
  dis version 3.00発表後にM.Suzuki氏からFreeBSD上でコンパイルするための差分をいただき、
  それを参考にして正式にFreeBSDに対応しました。
* Windows: Cygwin
* Windows: Cygwin -mno-cygwin (WinGW)
* Windows: MinGW

## 上記以外のOSへの移植

以下の調整をして下さい。

* ソースファイルの文字コードがUTF-8なので、文字コードを変換するかコンパイラのオプションで指定します。
* ビッグエンディアン環境でマクロ`__BIG_ENDIAN__`が定義されない場合はMakefileで定義します。
* `strtox()`など標準的でない関数の有無でMakefileを書き換えます。
* `char`を`unsigned char`として扱うように、コンパイラのオプションで指定します。

## 非対応環境

* OS-9/X680x0

OS-9/X680x0用の実行ファイルを作成することはできませんが、マクロ`OSKDIS`を定義すれば
OS-9/X680x0用のモジュールを逆アセンブルする実行ファイルが作成されます。

## Human68k

完全な再コンパイルには、`long double`(12バイト)に対応している`sprintf()`と、
文字列を`long double`に変換する`strtox()`が必要です。

`strtox()`はHASのfexpr.sからコードを抜き出すと楽に作れますが、`sprintf()`が`long double`に
対応していないと意味がありません。

### (1) ソースコードの文字コード変換

srcディレクトリのファイルはUTF-8エンコーディングなので、Shift_JISに変換します。

u8tosjコマンドがある場合はリポジトリのルートでmakeを実行すれば、変換されたファイルが
buildディレクトリに作成されます。


### (2) ライブラリの設定

Makefile の LIBFLAGS を、持っていないライブラリをリンクしないように修正します。
* libld.a がない場合 …… -lld を削除します
* libhdi.a がない場合 …… -lhdi を削除します
* libhmem.a がない場合 …… -lhmem を削除します

なお、libioctl.a を持っている人は -lioctl を追加するのも良いでしょう。

### (3) sprintf()のlong double対応有無

`sprintf()`が`long double`に対応していない場合は、Makefileの`FUNCCFLAGS`に`-DNO_PRINTF_LDBL`を追加します
(LIBCやlibhdiは対応していません)。

### (4) strtox()の有無

`strtox()`がない場合は`FUNCCFLAGS`から`-DHAVE_STRTOX`を削除します。

更に、fpconv.c:fpconv_x()の以下の部分を調整します。
```
#ifndef HAVE_STRTOX
  if (e <= 64) return FALSE;  // ライブラリに依存
#endif
```

ただし、`-DNO_PRINTF_LDBL`を追加した場合は使わないのでそのままで構いません。

### (5) make

make を実行します。

----
