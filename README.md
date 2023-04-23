# ソースコードジェネレータ for X680x0
 X680x0/Human68k 用の高機能逆アセンブラです。


## Build
PCやネット上での取り扱いを用意にするために、src/内のファイルはUTF-8で記述されています。
X680x0上でビルドする際には、UTF-8からShift_JISへの変換が必要です。

### u8tosjを使用する方法

あらかじめ、[u8tosj](https://github.com/kg68k/u8tosj)をビルドしてインストールしておいてください。

トップディレクトリで`make`を実行してください。以下の処理が行われます。
1. build/ディレクトリの作成。
2. src/内の各ファイルをShift_JISに変換してbuild/へ保存。

次に、カレントディレクトリをbuild/に変更し、`make`を実行してください。
実行ファイルが作成されます。

### u8tosjを使用しない方法

ファイルを適当なツールで適宜Shift_JISに変換してから`make`を実行してください。
UTF-8のままでは正しくビルドできませんので注意してください。


## License
GNU General Public License version 3 or later.


## Author
TcbnErik / 立花@桑島技研  
https://github.com/kg68k/dis
