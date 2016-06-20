SimulatedAnnealing.c
======================
講義内課題用  
SimulatedAnnealing.c:  
 通常版、出力先ファイルはresult.bat  
SimulatedAnnealing_S.c:  
 通常版のターミナル内の出力方法と出力先ファイル変更したもの。  

使い方
------
### SAを用いて問題を解く ###
    $ gcc SimulatedAnnealing.c -lm　　
    $./a.out <tsp_filename>　
結果はカレントディレクトリ内、result.datとして保存されます。　　

###画像の生成###
    /*コンパイル、実行にはgdライブラリが必要です*/  
    $  gcc draw.c -o draw -lm -lgd  
    $  ./draw tspファイル名(hoge.tsp) < 巡回路ファイル名(result.dat) > 出力先ファイル名(hoge.png)  
