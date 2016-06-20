巡回セールスマン問題用プログラム
======================
講義内課題用　　
SimulatedAnnealing.c:　　
 通常版、出力先ファイルはresult.bat　　
SimulatedAnnealing_S.c:　　
通常版のターミナル内の出力方法と出力先ファイルをtour1.bat(初期),tour2.bat(最適化後)と別々に出力するようにしたもの　　

使い方
------
### SAを用いて問題を解く ###
コンパイル　　
    $ gcc SimulatedAnnealing.c -lm　　
実行　　
    $./a.out <tsp_filename>　
結果はカレントディレクトリ内、result.datとして保存されます。　　

###画像の生成###
要gdライブラリ　　
    $  gcc draw.c -o draw -lm -lgd　　
    $  ./draw tspファイル名(hoge.tsp) < 巡回路ファイル名(result.dat) > 出力先ファイル名(hoge.png)　　
