# コンパイル方法

## Linux

```
git clone https://github.com/kg68k/dis.git
cmake -S dis -B build
cmake --build build
```

## Windows: Visual Studio 2022

「ローカルフォルダーを開く」で dis フォルダを開きます。
CMake でプロジェクトが構築されるのでそのままビルドしてください。

ただしそのままだとデバッグビルドになるので、リリースビルドを行う場合は下記手順で設定します。

1. 「プロジェクト」メニュー → dis の CMake の設定を開きます
   (または構成リストボックスの「構成を管理します...」)。
2. 新規構成の追加 → 「x64-Release」を選択します。
3. 構成の種類を「Release」(または「MinSizeRel」)に変更して保存します。
4. 構成リストボックスで「x64-Release」を選択します。

なお、スタートメニューの Developer Command Prompt for VS 2022 でコマンドプロンプトを開き、
Linux と同様の手順で CMake によるビルドを行うこともできます。
* git が使えない場合は代わりに手動でリポジトリのダウンロード、展開をします。
* リリースビルドの場合は `cmake --build build --config Release`
  (または `--config MinSizeRel`)とします。

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
