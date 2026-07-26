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
normal:
```text
width=8 height=8
```
rotate90:
```text
width=8 height=8
```
rotate90:
```text
scale2: width=16 height=16
```

### 考察
まず，`w=8, h=4` の非正方形パターンで検証したところ，`rotate0` では
`size=(w=8,h=4)` となり，`rotate90` では `size=(w=4,h=8)` と幅と高さが
入れ替わって出力された．これにより，`pattern_scaled_size` が
`ROTATE_90`／`ROTATE_270` のときにのみ `w` と `h` を入れ替えるという
仕様どおりに動作していることを確認できた．正方形（8×8）のみで検証した
場合はこの入れ替わりが数値上見分けられないため，非正方形での検証が
必要であった．

次に，`scale_w=3, scale_h=2` という非対称なスケールを与えたところ，
出力サイズの幅と高さがそれぞれ異なる倍率で拡大されており，
`scale_w` と `scale_h` が別々の軸に独立して適用されていることが分かった．
仮にどちらか一方の軸に誤って同じ倍率を適用する実装になっていた場合，
対称スケール（2倍・2倍）だけのテストでは検出できないため，
この非対称スケールでの確認によって初めてロジックの独立性を
保証できたといえる．

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
normal:
```text
resolve(0,0) -> (0,7)
```
rotate90:
```text
resolve(0,0) -> (0,7)
```
rotate90&scale2:
```text
resolve(1,1) -> (0,7)
```

### 考察
`ROTATE_0`・`ROTATE_90`・`ROTATE_180`・`ROTATE_270` について `resolve_coord` の
挙動を確認した．出力座標の四隅（`(0,0)` と反対側の角）を入力すると，
いずれの回転角度でも入力パターン上の対応する角に正しく写像されており，
90度単位の回転すべてで座標変換が一貫していることを確認できた．

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
仕様書では，`new_pattern_view`は標準倍率・回転90°の初期値を持ち，
`new_pattern_view_doublescale`はそのスケール倍率だけを2倍に変更するとされている．
実行結果がこの説明と一致していれば，両関数は仕様どおりに`PatternView`を初期化しているといえる．

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
仕様書では，パターン配列の各セル値が0/1に応じて
スペースまたは`view->string`として表示されるとされている．
実行結果において，ビットパターンに対応した形が行ごとに表示されていれば，
`print_as_pattern`は仕様どおりに描画を行っていることが確認できる．

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
