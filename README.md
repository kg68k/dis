# ソースコードジェネレータ for X680x0

X680x0/Human68k 用の高機能逆アセンブラです。  


## Build on X680x0

PCやネット上での取り扱いを用意にするために、src/内のファイルはUTF-8で記述されています。
X68000上でビルドする際には、UTF-8からShift_JISへの変換が必要です。

### src2buildを使用する場合

必要ツール: [src2build](https://github.com/kg68k/src2build)

srcディレクトリのある場所で以下のコマンドを実行します。
```
src2build src
make -C build
```

### その他の方法

src/内のファイルを適当なツールで適宜Shift_JISに変換して別のディレクトリに保存し、
ディレクトリ内で`make`を実行してください。  
UTF-8のままでは正しくビルドできません。


## License
GNU General Public License version 3 or later.


## Author
TcbnErik / https://github.com/kg68k/dis
