1) solve.c のコンパイル

$ gcc solve.c -o solve -lm

2) 実行

$ ./solve tspファイル名 

で実行すると tour1.dat 及び tour2.dat というファイルができる. 
これらのファイルには, それぞれ最近近傍法による巡回路, 及び, 
それに対して2-opt法を実行した結果が書かれている. 
(注: ただし配布したプログラムの2-optは何もしないので, 
tour1.dat = tour2.dat となっている. 
)

例)

$ ./solve in_data/instance10-1.tsp

3) 図で確認 (先週の課題2のプログラムを使う)
3) 図で確認 
キットの中の draw.c をコンパイル
$  gcc draw.c -o draw -lm -lgd
して事前に実行ファイル draw を生成しておく.

$ ./draw tspファイル名 < 巡回路ファイル > 画像ファイル名

例)

$ ./draw in_data/instance10-1.tsp < tour1.dat > out1.png
$ ./draw in_data/instance10-1.tsp < tour2.dat > out2.png
$ eog out1.png
$ eog out2.png


