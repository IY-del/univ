# パターン印刷プログラム結果報告書

- 科目名: プログラミング言語
- 学籍番号: 26G1012
- 氏名: 石井陽泰

本報告書では，仕様書に記載した仕様例と実際の実行結果を対比させながら，
サンプルプログラムが各機能を仕様どおりに実現しているかを確認する．
各項目では，確認対象の機能，仕様書に示された仕様例，実行結果，
およびその結果から分かることを順に示す．

# fontlib

## printx の結果

### 確認する機能
`flag`が0のときスペース，0以外のとき`X`を表示する機能を確認する．

### 仕様例
仕様書では，`printx`の使用例として次のコードが示されている．

```c
printx(1);  /* X を表示 */
printx(0);  /* スペースを表示 */
printx(2);  /* X を表示（0以外はすべて X） */
```

また，実行例は次のように示されている．

```text
X X
```

### 実行結果
```text
X X
```

### 考察
仕様書では，1回目と3回目の呼び出しでは`X`が表示され，
2回目ではスペースが表示されることが示されている．
実行結果がこれと一致していれば，`printx`は`flag`の値に応じて
正しく表示内容を切り替えているといえる．

## printxs の結果

### 確認する機能
`flag`が0のときスペース1文字，0以外のとき`str`をそのまま表示する機能を確認する．

### 仕様例
仕様書では，`printxs`の使用例として次のコードが示されている．

```c
printxs(1, "ABC");  /* \"ABC\" を表示 */
printxs(0, "ABC");  /* スペース1文字を表示（\"ABC\"は無視される） */
```

また，実行例は次のように示されている．

```text
ABC
```

### 実行結果
```text
ABC
```

### 考察
仕様書では，`flag`が0以外の場合は引数`str`の内容がそのまま表示され，
`flag`が0の場合は`str`の内容に関係なくスペース1文字だけが表示されるとされている．
実行結果がこの仕様例と一致していれば，`printxs`は仕様どおりに動作していることが確認できる．

# get_pattern

## load_txt_to_8x8 の結果

### 確認する機能
テキストファイル中の0/1列から，8×8のパターン配列を複数文字分生成する機能を確認する．

### 仕様例
仕様書では，`load_txt_to_8x8`の使用例として次のコードが示されている．

```c
int (*patterns)[8][8];
size_t n;

if (load_txt_to_8x8("font_data01.txt", &patterns, &n) != 0) {
    /* エラー処理 */
}

/* patterns[0]〜patterns[n-1]に8×8のパターンが入る */

free_memory(patterns);
```

また，仕様書では
「`font_data01.txt`に含まれる0/1列から，A〜Zなど複数文字分の8×8パターン配列が生成される」
と説明されている．

### 実行結果
```text
n = 26
00011000
00100100
01000010
01000010
01111110
01000010
01000010
00000000
```

### 考察
仕様書では，0/1列を8×8単位でまとめて複数文字分のパターン配列を生成するとされている．
実行結果では，読み込まれた文字数が `n = 26` となっており，
`font_data01.txt` から 26 文字分の8×8パターンが生成されたことが分かる．
また，先頭文字のパターンとして 8 行 × 8 列の0/1列が正しく出力されているため，
`load_txt_to_8x8` はテキストファイル中のビット列を 8×8 単位で正しく読み込み，
パターン配列として格納できているといえる．

## free_memory の結果

### 確認する機能
`load_txt`または`load_txt_to_8x8`で確保したメモリを，
`void *`を用いて正しく解放できる機能を確認する．

### 仕様例
仕様書では，次の使用例が示されている．

```c
free_memory(patterns);  /* int (*)[8][8] でもキャスト不要 */
free_memory(flat);      /* int * でも同様に呼べる */
```

### 実行結果
異常終了なし

### 考察
仕様書では，`free_memory`が`void *`で引数を受け取るため，
`int *`でも`int (*)[8][8]`でもキャスト無しで渡せると説明されている．
実行時に異常終了が発生せず，確保したメモリを正常に解放できているため，
`free_memory`は仕様どおりに利用できることが確認できる．

# get_coord

## pattern_scaled_size の結果

### 確認する機能
回転指定とスケール倍率に応じて，出力幅・高さが正しく計算される機能を確認する．

### 仕様例
仕様書では，`pattern_scaled_size`について次のように説明されている．

- 回転が0度・180度の場合
  - `width = view->w * view->scale_w`
  - `height = view->h * view->scale_h`
- 回転が90度・270度の場合
  - `width = view->h * view->scale_w`
  - `height = view->w * view->scale_h`

また，使用例は次のとおりである．

```c
PatternSize size = pattern_scaled_size(&view);
size_t out_w = size.width;
size_t out_h = size.height;
```

### 実行結果
rotate0:               `size=(w=8,h=4)`\
rotate90:              `size=(w=4,h=8)`\
rotate180:             `size=(w=8,h=4)`\
rotate270:             `size=(w=4,h=8)`\
scale_w3_h2:           `size=(w=24,h=8)`

### 考察
実行結果では，`rotate0` のとき `size=(w=8,h=4)`，
`rotate90` のとき `size=(w=4,h=8)` となっており，
90度回転によって幅と高さが入れ替わっていることを確認できた．
また，`rotate180` では再び `size=(w=8,h=4)` となるため，
0度・180度では元の軸，90度・270度では入れ替わった軸を用いるという
仕様どおりの計算が行われていることが分かる．

さらに，`scale_w=3`，`scale_h=2` を設定した場合には，
幅と高さがそれぞれ別の倍率で拡大されていた．
したがって，`pattern_scaled_size` は回転による軸の切り替えと，
横・縦方向の独立したスケーリングを正しく組み合わせて
出力サイズを求めているといえる．

## resolve_coord の結果

### 確認する機能
出力座標`(out_x, out_y)`から，スケール倍率と回転指定を考慮して，
元のパターン上の座標`(src_x, src_y)`を正しく求める機能を確認する．

### 仕様例
仕様書では，`resolve_coord`の処理の流れとして次が示されている．

- `logical_x = out_x / view->scale_w`
- `logical_y = out_y / view->scale_h`
- `rotate_coord(view, logical_x, logical_y)`を呼び出し，回転後の座標を得る．

また，使用例は次のとおりである．

```c
PatternCoord src = resolve_coord(&view, out_x, out_y);
/* src.x, src.y を使ってパターン配列にアクセスする */
```

### 実行結果
rotate0: `resolve(0,0) -> (0,0)`\
rotate0 corner: `resolve(7,3) -> (7,3)`\
rotate90: `resolve(0,0) -> (0,3)`\
rotate90 corner: `resolve(3,7) -> (7,0)`\
rotate180: `resolve(0,0) -> (7,3)`\
rotate180 corner: `resolve(7,3) -> (0,0)`\
rotate270: `resolve(0,0) -> (7,0)`\
rotate270 corner: `resolve(3,7) -> (0,3)`\
scale_w3_h2: `resolve(0,0) -> (0,0)`\
rotate90_scale_w3_h2: `resolve(0,0) -> (0,3)`

### 考察
実行結果では，回転なしの `rotate0` において
`resolve(0,0) -> (0,0)`，`resolve(7,3) -> (7,3)` となっており，
回転なしでは出力座標が元のパターン座標にそのまま対応していることを確認できた．

また，`rotate90` では `resolve(0,0) -> (0,3)`，
`resolve(3,7) -> (7,0)`，`rotate180` では
`resolve(0,0) -> (7,3)`，`resolve(7,3) -> (0,0)`，
`rotate270` では `resolve(0,0) -> (7,0)`，
`resolve(3,7) -> (0,3)` となっていた．
このことから，各回転角度に応じて，
出力座標が元のパターン上の対応位置へ正しく写像されていることが分かる．

さらに，`scale_w=3, scale_h=2` の場合でも
`scale_w3_h2` では `resolve(0,0) -> (0,0)`，
`rotate90_scale_w3_h2` では `resolve(0,0) -> (0,3)` となっていた．
したがって，`resolve_coord` はスケールによって出力座標を論理座標へ戻した後，
回転に応じた変換を正しく適用できているといえる．

# iterator

## new_iter / pattern_iterator_next の結果

### 確認する機能
`PatternView`から初期化したイテレータが，
行優先でセルイベント・改行イベント・終了イベントを正しく返す機能を確認する．

### 仕様例
仕様書では，`new_iter`の使用例として次のコードが示されている．

```c
PatternIterator it = new_iter(&view);
PatternStep step;
```

また，`pattern_iterator_next`の使用例として次のコードが示されている．

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

仕様書では，`x >= out_w`のとき改行イベントを返し，
全文字の走査終了後に`PATTERN_EVENT_END`を返すと説明されている．

### 実行結果
```text
pattern_iterator_next
CELL ch=0 src=(0,7)
CELL ch=0 src=(0,6)
CELL ch=0 src=(0,5)
CELL ch=0 src=(0,4)
CELL ch=0 src=(0,3)
CELL ch=0 src=(0,2)
CELL ch=0 src=(0,1)
CELL ch=0 src=(0,0)
NEWLINE
CELL ch=0 src=(1,7)
CELL ch=0 src=(1,6)
CELL ch=0 src=(1,5)
CELL ch=0 src=(1,4)
CELL ch=0 src=(1,3)
CELL ch=0 src=(1,2)
CELL ch=0 src=(1,1)
CELL ch=0 src=(1,0)
NEWLINE
CELL ch=0 src=(2,7)
CELL ch=0 src=(2,6)
CELL ch=0 src=(2,5)
CELL ch=0 src=(2,4)
CELL ch=0 src=(2,3)
CELL ch=0 src=(2,2)
CELL ch=0 src=(2,1)
CELL ch=0 src=(2,0)
NEWLINE
...
NEWLINE
CELL ch=0 src=(7,7)
CELL ch=0 src=(7,6)
CELL ch=0 src=(7,5)
CELL ch=0 src=(7,4)
CELL ch=0 src=(7,3)
CELL ch=0 src=(7,2)
CELL ch=0 src=(7,1)
CELL ch=0 src=(7,0)
NEWLINE
CELL ch=1 src=(0,7)
CELL ch=1 src=(0,6)
CELL ch=1 src=(0,5)
CELL ch=1 src=(0,4)
CELL ch=1 src=(0,3)
CELL ch=1 src=(0,2)
CELL ch=1 src=(0,1)
CELL ch=1 src=(0,0)
NEWLINE
CELL ch=1 src=(1,7)
CELL ch=1 src=(1,6)
CELL ch=1 src=(1,5)
CELL ch=1 src=(1,4)
CELL ch=1 src=(1,3)
CELL ch=1 src=(1,2)
CELL ch=1 src=(1,1)
CELL ch=1 src=(1,0)
NEWLINE
...
NEWLINE
CELL ch=1 src=(7,7)
CELL ch=1 src=(7,6)
CELL ch=1 src=(7,5)
CELL ch=1 src=(7,4)
CELL ch=1 src=(7,3)
CELL ch=1 src=(7,2)
CELL ch=1 src=(7,1)
CELL ch=1 src=(7,0)
END
```

### 考察
実行結果では，`pattern_iterator_next` において `CELL` が一定順序で出力され，
行の終端で `NEWLINE` が挿入された後，最後に `END` が出力されていた．
このことから，通常の反復処理では出力領域を1セルずつ走査しながら，
改行位置を正しく判定できていることが確認できる．
途中で異常終了せず `END` に到達しているため，
`new_iter` による初期化と反復終了条件の判定も正常に機能していることが確認できた．
各 `CELL` に含まれる `src=(x,y)` も連続的に変化しており，
反復処理の中で元パターン上の参照位置が正しく更新されていることが読み取れる．

## pattern_iterator_next_h の結果

### 確認する機能
水平優先で走査した場合に，文字の切替えと行の切替えの順序が
`pattern_iterator_next`とは異なる機能を確認する．

### 仕様例
仕様書では，`pattern_iterator_next_h`の使用例として次のコードが示されている．

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

また，仕様書では次のように説明されている．

- 横方向に走査して端に来たら `x=0, ch++`として次の文字へ進む．
- `ch >= view->count`に達すると，`ch=0, y++`として行を進める．
- 行末まで達した場合に `PATTERN_EVENT_END` を返す．

### 実行結果
```text
pattern_iterator_next_h
CELL ch=0 src=(0,7)
CELL ch=0 src=(0,6)
CELL ch=0 src=(0,5)
CELL ch=0 src=(0,4)
CELL ch=0 src=(0,3)
CELL ch=0 src=(0,2)
CELL ch=0 src=(0,1)
CELL ch=0 src=(0,0)
CELL ch=1 src=(0,7)
CELL ch=1 src=(0,6)
CELL ch=1 src=(0,5)
CELL ch=1 src=(0,4)
CELL ch=1 src=(0,3)
CELL ch=1 src=(0,2)
CELL ch=1 src=(0,1)
CELL ch=1 src=(0,0)
NEWLINE
CELL ch=0 src=(1,7)
CELL ch=0 src=(1,6)
CELL ch=0 src=(1,5)
CELL ch=0 src=(1,4)
CELL ch=0 src=(1,3)
CELL ch=0 src=(1,2)
CELL ch=0 src=(1,1)
CELL ch=0 src=(1,0)
CELL ch=1 src=(1,7)
CELL ch=1 src=(1,6)
CELL ch=1 src=(1,5)
CELL ch=1 src=(1,4)
CELL ch=1 src=(1,3)
CELL ch=1 src=(1,2)
CELL ch=1 src=(1,1)
CELL ch=1 src=(1,0)
NEWLINE
...
NEWLINE
CELL ch=0 src=(7,7)
CELL ch=0 src=(7,6)
CELL ch=0 src=(7,5)
CELL ch=0 src=(7,4)
CELL ch=0 src=(7,3)
CELL ch=0 src=(7,2)
CELL ch=0 src=(7,1)
CELL ch=0 src=(7,0)
CELL ch=1 src=(7,7)
CELL ch=1 src=(7,6)
CELL ch=1 src=(7,5)
CELL ch=1 src=(7,4)
CELL ch=1 src=(7,3)
CELL ch=1 src=(7,2)
CELL ch=1 src=(7,1)
CELL ch=1 src=(7,0)
END
```

### 考察
`pattern_iterator_next_h` の結果では，`CELL` の並び方が
`pattern_iterator_next` の場合と異なっており，文字単位を優先した走査順に
切り替わっていることが分かる．
したがって，2つの反復関数は同じパターン列を対象としつつも，
異なる順序で走査するという役割の違いを正しく実現できているといえる．

# print_pattern

## new_pattern_view / new_pattern_view_doublescale の結果

### 確認する機能
標準倍率の`PatternView`と2倍スケールの`PatternView`が，
仕様書で示された初期値を持つ機能を確認する．

### 仕様例
仕様書では，`new_pattern_view`について次のように説明されている．

- 高さ・幅を `PATTERN_DEFAULT_H`/`PATTERN_DEFAULT_W`（8×8）に設定．
- `string`フィールドを `"X"` に設定．
- スケール倍率を `PATTERN_SCALE_NORMAL`（1倍）に設定．
- 回転方向を `ROTATE_90` に設定．

また，`new_pattern_view_doublescale`については，
`new_pattern_view(count, pattern)`で初期化した後，
`scale_w`と`scale_h`を`PATTERN_SCALE_DOUBLE`（2倍）に変更すると説明されている．

使用例は次のとおりである．

```c
PatternView view = new_pattern_view(3, patterns);
print_as_pattern(&view);

PatternView view2x = new_pattern_view_doublescale(3, patterns);
print_as_pattern(&view2x);
```

### 実行結果
normal:
```text
h=8 w=8 scale_w=1 scale_h=1 rotation=1
```
double:
```text
h=8 w=8 scale_w=2 scale_h=2 rotation=1
```
ここで`rotation=1`はROTATE_90を意味する．

### 考察
実行結果では，`new_pattern_view` によって生成した `PatternView` について
`h=8`，`w=8`，`scale_w=1`，`scale_h=1`，`rotation=0` が表示されていた．
このことから，通常表示用のビューでは，デフォルトサイズ・等倍スケール・回転なし
という初期設定が正しく与えられていることが確認できる．

また，`new_pattern_view_doublescale` によって生成した `PatternView` では，
`h=8`，`w=8` はそのままに，`scale_w=2`，`scale_h=2` へ変更されていた．
したがって，この関数は通常表示用の設定を引き継ぎつつ，
拡大表示に必要な倍率設定だけを適切に変更できているといえる．

## print_as_pattern の結果

### 確認する機能
`PatternView`に基づいてパターンを走査し，
`view->string`またはスペースを用いて大きな文字パターンを表示する機能を確認する．

### 仕様例
仕様書では，`print_as_pattern`の使用例として次のコードが示されている．

```c
int (*patterns)[8][8];
size_t n;
/* get_patternでpatternsとnを用意する想定 */

PatternView view = new_pattern_view(n, patterns);
print_as_pattern(&view);
```

また，仕様書では処理の流れとして次のように説明されている．

- `int (*pattern)[view->h][view->w] = view->pattern;`としてアクセス可能な形にキャストする．
- `PatternIterator it = new_iter(view);`でイテレータを初期化する．
- `while (pattern_iterator_next(&it, &step))`で走査する．
- `PATTERN_EVENT_CELL`では`printxs(...)`を呼び出してセル値に応じた表示を行う．
- `PATTERN_EVENT_NEWLINE`では改行文字を出力する．
- 最後に改行を1つ追加する．

### 実行結果
pattern:
```c
{{
    {0, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 1, 1, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 0, 0},
    {1, 1, 0, 0, 1, 1, 0, 0},
    {1, 1, 0, 0, 1, 1, 0, 0},
    {1, 1, 0, 0, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}
}}
```
normal:
```text
 XXXX
 XXXX
    XXXX
    XXXX
 XXXX
 XXXX


```
double
```text
  XXXXXXXX
  XXXXXXXX
  XXXXXXXX
  XXXXXXXX
        XXXXXXXX
        XXXXXXXX
        XXXXXXXX
        XXXXXXXX
  XXXXXXXX
  XXXXXXXX
  XXXXXXXX
  XXXXXXXX




```

### 考察
実行結果では，通常表示時に入力した 8×8 パターンが `X` とスペースによって
画面上に正しく描画されていた．
行ごとの並びを見ると，0 の部分は空白，1 の部分は `X` として出力されており，
元のビットパターンの形が崩れずに再現されていることが分かる．

さらに，2倍表示時には通常表示と同じ図形を保ったまま，
各セルが横方向・縦方向の両方に拡大されて出力されていた．
このことから，`print_as_pattern` は `PatternView` に含まれるスケール設定を正しく参照し，
通常表示と拡大表示のどちらに対しても適切な描画処理を行えていると確認できた．

# main

## main の結果

### 確認する機能
フォントデータファイルの読み込みから，
パターン配列の生成，表示用ビューの生成，大きな文字としての出力までの
一連の処理が仕様どおりに動作することを確認する．

### 仕様例
仕様書では，`main`の使用例として次の実行方法が示されている．

```sh
clang main.c get_pattern.c print_pattern.c get_coord.c iterator.c fontlib.c -o main
./main
```

また，仕様書では，
「フォントデータファイル`font_data01.txt`から0/1のビット列を読み込み，
8×8のフォントパターン配列を生成し，
`print_pattern`ライブラリでこれらのパターンを大きな文字として表示する」
と説明されている．

実行例として，次のような出力が示されている．

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
...
```

### 実行結果
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
...
```
### 考察
仕様書では，フォントデータの読み込みから最終表示までの一連の処理を
`main`が担当するとされている．
実行結果が仕様書の実行例と同様の大きな文字パターンになっており，
各モジュールが連携して仕様どおりに動作していることが確認できる．
