# fontlib

## 概要

`fontlib`は，指定されたフラグに応じて「X」またはスペースを1文字表示する関数`printx`と，  
任意の文字列またはスペース1文字を表示する関数`printxs`を提供する簡易フォント表示ライブラリである．
他のモジュールから「1セルの描画」や「文字列の描画」を呼び出しやすくする基礎的な表示機能を提供する．

## コンパイル方法

ソースコードが`foo.c`の場合，以下のようにコンパイルする．

### 静的ライブラリ

静的ライブラリの作成

```sh
$ gcc -c fontlib.c
$ ar rcs libfont.a fontlib.o
```

`foo.c`のコンパイル

```sh
$ gcc foo.c -L. -lfont
```

### 動的ライブラリ

静的ライブラリの作り方に準じて，以下のように動的ライブラリを作成する．

```sh
# 位置独立コードでコンパイル
$ gcc -fPIC -c fontlib.c

# 共有ライブラリの作成
$ gcc -shared -o libfont.so fontlib.o

# foo.cのコンパイル
$ gcc foo.c -L. -lfont

# 実行時にライブラリパスを通す
$ LD_LIBRARY_PATH=. ./a.out
```

## 関数一覧

関数名 | 説明
-|-
printx | フラグに応じて「X」またはスペースを1文字表示する
printxs | フラグに応じて文字列そのもの，またはスペース1文字を表示する

## printx

パラメータ`flag`が0ならスペース（`" "`）を，  
0でない値なら`"X"`を1文字表示する関数である．

パラメータ名 | 型 | 意味
-|-|-
flag | int | 表示内容を決定するフラグ（0でスペース，それ以外で`X`）

戻り値 | 型 | 意味
-|-|-
戻り値 | void | 無し

### 使用例

```c
printx(1);  /* X を表示 */
printx(0);  /* スペースを表示 */
printx(2);  /* X を表示（0以外はすべて X） */
```

### 実行例

```text
X X
```



## printxs

パラメータ`flag`が0ならスペース（`" "`）を1文字だけ表示し，  
0でない値なら，引数`str`で与えられた文字列をそのまま表示する関数である．

パラメータ名 | 型 | 意味
-|-|-
flag | int | 表示内容を決定するフラグ（0でスペース，それ以外で`str`を表示）
str | const char[] | 表示対象となる文字列

戻り値 | 型 | 意味
-|-|-
戻り値 | void | 無し

### 使用例

```c
printxs(1, "ABC");  /* \"ABC\" を表示 */
printxs(0, "ABC");  /* スペース1文字を表示（\"ABC\"は無視される） */
```

### 実行例

```text
ABC 
```



# get_pattern

## 概要

`get_pattern`は，テキストファイルからフォントのビットパターン（0/1列）を読み込み，  
8×8のint配列へパディングした形で格納する機能を提供するライブラリである．
ファイル中の「0/1」を連続的に読み取り，64個ごとに1文字分（8×8）としてまとめることで，  
複数文字分のフォントパターン配列を生成する．

## コンパイル方法

### 静的ライブラリ

```sh
$ gcc -c get_pattern.c
$ ar rcs libgetpattern.a get_pattern.o
$ gcc foo.c -L. -lgetpattern
```

### 動的ライブラリ

```sh
$ gcc -fPIC -c get_pattern.c
$ gcc -shared -o libgetpattern.so get_pattern.o
$ gcc foo.c -L. -lgetpattern
$ LD_LIBRARY_PATH=. ./a.out
```


## 関数一覧

関数名 | 説明
-|-
load_txt_to_padded_8x8 | テキストファイルから 8×8 パターン配列を読み込む
free_padded_8x8 | `load_txt_to_padded_8x8`で確保したパターン配列を解放する

## load_txt_to_padded_8x8

テキスト形式のフォントデータファイルを読み込み，  
ファイル中の'0'および'1'文字を抽出して 8×8 のint配列に詰める関数である．

処理の流れ：
- `filename`を読み取り専用で開く．  
- ファイルから1文字ずつ読み込み，'0'または'1'であれば内部バッファに格納する．  
- バッファ長`len`に対して，64要素ごとに1文字分のパターンとみなし，  
  - `n = (len + 63) / 64`個分の 8×8 パターンを`calloc`で確保する．  
- 各要素を `arr[i / 64][(i % 64) / 8][i % 8]` に書き込むことで，  
  1次元の0/1列を2次元に再配置する．  
- `out`と`out_n`に確保した配列のポインタと文字数を返す．

パラメータ名 | 型 | 意味
-|-|-
filename | const char * | フォントデータファイル名
out | int (**) | 出力されるパターン配列へのポインタ（ポインタ変数へのポインタ）
out_n | size_t * | 読み込んだパターン数（文字数）

戻り値 | 型 | 意味
-|-|-
戻り値 | int | 成功時0，ファイルオープン失敗時-1，メモリ確保失敗時-2

### 使用例

```c
int (*patterns);
size_t n;

if (load_txt_to_padded_8x8("font_data01.txt", &patterns, &n) != 0) {
    /* エラー処理 */
}

/* patterns[0]〜patterns[n-1]に8×8のパターンが入る */

free_padded_8x8(patterns);
```

### 実行例（イメージ）

`font_data01.txt`に含まれる0/1列から，A〜Zなど複数文字分の8×8パターン配列が生成される．



## free_padded_8x8

`load_txt_to_padded_8x8`で確保した8×8パターン配列を解放する関数である．

パラメータ名 | 型 | 意味
-|-|-
arr | int (*arr) | 解放対象のパターン配列

戻り値 | 型 | 意味
-|-|-
戻り値 | void | 無し

### 使用例

```c
free_padded_8x8(patterns);
```



# get_coord

## 概要

`get_coord`は，`PatternView`構造体に基づいて，出力上の座標から  
元のパターン上の座標への変換を行うライブラリである．
回転指定（0,90,180,270度）とスケール倍率（拡大率）を考慮した座標変換を提供し，  
パターン走査処理から利用される．

## コンパイル方法

### 静的ライブラリ

```sh
$ gcc -c get_coord.c
$ ar rcs libcoord.a get_coord.o
$ gcc foo.c -L. -lcoord
```

### 動的ライブラリ

```sh
$ gcc -fPIC -c get_coord.c
$ gcc -shared -o libcoord.so get_coord.o
$ gcc foo.c -L. -lcoord
$ LD_LIBRARY_PATH=. ./a.out
```


## 関数一覧

関数名 | 説明
-|-
pattern_output_width | 回転後の論理的なパターン幅を返す
pattern_output_height | 回転後の論理的なパターン高さを返す
pattern_scaled_width | スケール適用後の出力幅を返す
pattern_scaled_height | スケール適用後の出力高さを返す
resolve_coord | 出力座標から元のパターン座標を求める

※`rotate_coord`は`static`な内部関数であり，仕様書では補助関数として扱う．

## pattern_output_width / pattern_output_height

`PatternView`の回転指定に応じて，表示時の論理的な幅・高さを返す関数である．

- 幅:  
  - 回転が0度・180度の場合 → `view->w`  
  - 90度・270度の場合 → `view->h`
- 高さ:  
  - 回転が0度・180度の場合 → `view->h`  
  - 90度・270度の場合 → `view->w`

パラメータ名 | 型 | 意味
-|-|-
view | const PatternView * | 対象となるパターンビュー

戻り値 | 型 | 意味
-|-|-
戻り値 | size_t | 回転後の幅/高さ

### 使用例

```c
size_t out_w = pattern_output_width(&view);
size_t out_h = pattern_output_height(&view);
```



## pattern_scaled_width / pattern_scaled_height

`pattern_output_width/height`にスケール倍率を掛け合わせた「出力上の」幅・高さを返す関数である．

- 幅: `pattern_output_width(view) * view->scale_w`
- 高さ: `pattern_output_height(view) * view->scale_h`

パラメータ名 | 型 | 意味
-|-|-
view | const PatternView * | 対象となるパターンビュー

戻り値 | 型 | 意味
-|-|-
戻り値 | size_t | スケール適用後の幅/高さ

### 使用例

```c
size_t out_w = pattern_scaled_width(&view);
size_t out_h = pattern_scaled_height(&view);
```



## resolve_coord

出力座標`(out_x, out_y)`から，スケール倍率を考慮して論理座標`(logical_x, logical_y)`を求め，  
さらに回転指定に応じて元のパターン座標`(src_x, src_y)`へ変換する関数である．

処理の流れ：
- `logical_x = out_x / view->scale_w`  
- `logical_y = out_y / view->scale_h`  
- `rotate_coord(view, logical_x, logical_y)`を呼び出し，回転後の座標を得る．

パラメータ名 | 型 | 意味
-|-|-
view | const PatternView * | 対象となるパターンビュー
out_x | size_t | 出力上のX座標
out_y | size_t | 出力上のY座標

戻り値 | 型 | 意味
-|-|-
戻り値 | PatternCoord | 元のパターン上の座標（x,y）

### 使用例

```c
PatternCoord src = resolve_coord(&view, out_x, out_y);
/* src.x, src.y を使ってパターン配列にアクセスする */
```



# iterator

## 概要

`iterator`は，`PatternView`に基づいてパターンを走査するためのイテレータ機能を提供するライブラリである．
出力上の座標を順に進めながら，各セルに対応するパターン上の座標や文字インデックスを`PatternStep`構造体として返す．

## コンパイル方法

### 静的ライブラリ

```sh
$ gcc -c iterator.c
$ ar rcs libiterator.a iterator.o
$ gcc foo.c -L. -literator
```

### 動的ライブラリ

```sh
$ gcc -fPIC -c iterator.c
$ gcc -shared -o libiterator.so iterator.o
$ gcc foo.c -L. -literator
$ LD_LIBRARY_PATH=. ./a.out
```


## 関数一覧

関数名 | 説明
-|-
new_iter | `PatternView`から新しいイテレータを初期化する
pattern_iterator_next | 行優先でパターンを走査する
pattern_iterator_next_h | 水平優先（文字を切り替える順序が異なる）でパターンを走査する

## new_iter

`PatternView`の情報をもとに，イテレータ構造体`PatternIterator`を初期化する関数である．

- 出力幅・高さを `pattern_scaled_width/view` で計算し，`out_w/out_h`に設定する．  
- 開始位置として，`x=0, y=0, ch=0`とする．

パラメータ名 | 型 | 意味
-|-|-
view | const PatternView * | 対象となるパターンビュー

戻り値 | 型 | 意味
-|-|-
戻り値 | PatternIterator | 初期化済みのイテレータ構造体

### 使用例

```c
PatternIterator it = new_iter(&view);
PatternStep step;
```



## pattern_iterator_next

出力上の座標を行優先で走査し，次のイベントを`PatternStep`に格納する関数である．

処理の流れ（概要）：
- `x >= out_w`の場合：  
  - `x = 0`にリセットし，`y++`して改行イベント`PATTERN_EVENT_NEWLINE`を返す．  
  - `y >= out_h`の場合：  
    - `y = 0`にリセットし，次の文字インデックス`ch++`へ進める．  
  - `ch >= view->count`に達すると，`PATTERN_EVENT_END`を返す．
- それ以外の場合：  
  - `resolve_coord(view, x, y)`で元座標`src`を求める．  
  - `step->ch`に文字インデックス，`step->src_x/src_y`に座標を設定し，  
    - `step->kind = PATTERN_EVENT_CELL` としてセルイベントを返す．  
  - `x++`して次のセルへ進む．

パラメータ名 | 型 | 意味
-|-|-
it | PatternIterator * | イテレータ状態
step | PatternStep * | 出力先のステップ情報

戻り値 | 型 | 意味
-|-|-
戻り値 | PatternEvent | イベント種別（CELL/NEWLINE/END）

### 使用例

```c
PatternIterator it = new_iter(&view);
PatternStep step;
while (pattern_iterator_next(&it, &step) != PATTERN_EVENT_END) {
    if (step.kind == PATTERN_EVENT_CELL) {
        /* step.ch, step.src_x, step.src_y を使って描画 */
    } else if (step.kind == PATTERN_EVENT_NEWLINE) {
        putchar('\n');
    }
}
```



## pattern_iterator_next_h

`pattern_iterator_next`と類似だが，文字の切り替えと行の切り替えの順序が異なるバリエーションである．

- 横方向に走査して端に来たら `x=0, ch++`として次の文字へ進む．  
- `ch >= view->count`に達すると，`ch=0, y++`として行を進める．  
- 行末まで達した場合に `PATTERN_EVENT_END` を返す．  
- セルイベント時には `resolve_coord`で座標を求め，`step`に格納する．

パラメータ名 | 型 | 意味
-|-|-
it | PatternIterator * | イテレータ状態
step | PatternStep * | 出力先のステップ情報

戻り値 | 型 | 意味
-|-|-
戻り値 | PatternEvent | イベント種別（CELL/NEWLINE/END）

### 使用例

```c
PatternIterator it = new_iter(&view);
PatternStep step;
while (pattern_iterator_next_h(&it, &step) != PATTERN_EVENT_END) {
    if (step.kind == PATTERN_EVENT_CELL) {
        /* 水平優先の描画に使う */
    } else if (step.kind == PATTERN_EVENT_NEWLINE) {
        putchar('\n');
    }
}
```



# print_pattern

## 概要

`print_pattern`は，フォントパターン配列と文字列情報をまとめた`PatternView`構造体と，  
その表示倍率および回転方向を指定するための定数群を提供し，  
`PatternIterator`を用いて画面へパターンを表示するライブラリである．

## コンパイル方法

### 静的ライブラリ

```sh
$ gcc -c print_pattern.c
$ ar rcs libprintpattern.a print_pattern.o
$ gcc foo.c -L. -lprintpattern
```

### 動的ライブラリ

```sh
$ gcc -fPIC -c print_pattern.c
$ gcc -shared -o libprintpattern.so print_pattern.o
$ gcc foo.c -L. -lprintpattern
$ LD_LIBRARY_PATH=. ./a.out
```


## 関数一覧

関数名 | 説明
-|-
new_pattern_view | パターンデータから標準倍率の`PatternView`を生成する
new_pattern_view_doublescale | パターンデータから2倍スケールの`PatternView`を生成する
print_as_pattern | `PatternView`の内容を画面に大きな文字パターンとして表示する

## new_pattern_view

標準倍率（スケール1倍）かつ回転なし（0度）で表示するための`PatternView`を初期化する関数である．

- 高さ・幅を `PATTERN_DEFAULT_H`/`PATTERN_DEFAULT_W`（8×8）に設定．  
- `string`フィールドを `"X"` に設定．  
- スケール倍率を `PATTERN_SCALE_NORMAL`（1倍）に設定．  
- 回転方向を `ROTATE_0` に設定．

パラメータ名 | 型 | 意味
-|-|-
count | size_t | パターン数（文字数など）
pattern | void * | パターンデータへのポインタ（`int[count]`など）

戻り値 | 型 | 意味
-|-|-
戻り値 | PatternView | 初期化済みのパターンビュー構造体

### 使用例

```c
int patterns[]; /* 8x8パターンの配列 */
PatternView view = new_pattern_view(3, patterns);
print_as_pattern(&view);
```



## new_pattern_view_doublescale

`new_pattern_view`で生成された`PatternView`をもとに，  
横・縦のスケール倍率を2倍に変更した`PatternView`を返す関数である．

- `new_pattern_view(count, pattern)`で初期化した後，  
  - `scale_w`と`scale_h`を`PATTERN_SCALE_DOUBLE`（2倍）に変更する．

パラメータ名 | 型 | 意味
-|-|-
count | size_t | パターン数
pattern | void * | パターンデータへのポインタ

戻り値 | 型 | 意味
-|-|-
戻り値 | PatternView | 2倍スケール用に初期化されたパターンビュー

### 使用例

```c
PatternView view2x = new_pattern_view_doublescale(3, patterns);
print_as_pattern(&view2x);
```



## print_as_pattern

`PatternView`に基づいて`PatternIterator`を初期化し，  
パターンの各セルを走査しながら画面に表示する関数である．

処理の流れ：
- `int (*pattern)[view->h][view->w] = view->pattern;`として，  
  - `pattern[ch][y][x]`で各セルにアクセス可能な形にキャストする．  
- `PatternIterator it = new_iter(view);`でイテレータを初期化．  
- `PatternStep step;`を用意し，`while (pattern_iterator_next(&it, &step))`で走査．  
  - `PATTERN_EVENT_CELL`の場合：  
    - `printxs(pattern[step.ch][step.src_y][step.src_x], view->string);`を呼び出し，  
      - セル値（0/1）に応じて，`view->string`またはスペースを表示する．  
  - `PATTERN_EVENT_NEWLINE`の場合：  
    - 改行文字を出力する．  
  - `PATTERN_EVENT_END`の場合：  
    - ループ終了．  
- 最後に改行を1つ追加する．

パラメータ名 | 型 | 意味
-|-|-
view | const PatternView * | 表示対象のパターンビュー

戻り値 | 型 | 意味
-|-|-
戻り値 | void | 無し

### 使用例

```c
int (*patterns);
size_t n;
/* get_patternでpatternsとnを用意する想定 */

PatternView view = new_pattern_view(n, patterns);
print_as_pattern(&view);
```

### 実行例（イメージ）

指定されたフォントパターンに基づいて，  
`view->string`（デフォルトでは`"X"`）を用いた大きな文字表示が行われる．

# main

## 概要

フォントデータファイル`font_data01.txt`から 0/1 のビット列を読み込み，  
`get_pattern`ライブラリで 8×8 のフォントパターン配列を生成し，  
`print_pattern`ライブラリでこれらのパターンを大きな文字として表示するエントリポイント関数である．[1][2][3]

## コンパイル方法

ソースコードが`main.c`の場合，以下のようにコンパイルする．[1]

```sh
clang main.c get_pattern.c print_pattern.c get_coord.c iterator.c fontlib.c -o main
```

## 関数一覧

関数名 | 説明
-|-
main | フォントデータを読み込み，パターンを画面に表示するプログラム本体

## main

フォントデータファイルから 8×8 のパターン配列を読み込み，  
`print_pattern`を用いて複数文字分のパターンを大きく表示する．

パラメータ名 | 型 | 意味
-|-|-
(無し) | - | コマンドライン引数を使用しない

戻り値 | 型 | 意味
-|-|-
戻り値 | int | 正常終了時 0，フォントデータ読み込み失敗時 非0

### 使用例

```sh
clang main.c get_pattern.c print_pattern.c get_coord.c iterator.c fontlib.c -o main
./main
```

### 実行例

```text
  XX            
  XX            
  XXXXXXXXXXXX  
  XXXXXXXXXXXX  
  XX    XX    XX
  XX    XX    XX
        XX    XX
        XX    XX
  XX    XX    XX
  XX    XX    XX
  XXXXXXXXXXXX  
  XXXXXXXXXXXX  
  XX            
  XX            
                
                
  XX          XX
  XX          XX
  XXXXXXXXXXXXXX
  XXXXXXXXXXXXXX
  XX    XX    XX
  XX    XX    XX
  XX    XX    XX
  XX    XX    XX
  XX    XX    XX
  XX    XX    XX
    XXXX  XXXX  
    XXXX  XXXX  
```

（以下，省略）  

この出力は，`font_data01.txt`のビット列を 8×8 パターンとして解釈し，  
`print_as_pattern`が仕様どおりに `X` とスペースを用いて表示していることを示す．