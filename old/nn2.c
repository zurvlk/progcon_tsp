/*                                                                */
/*    経路構築用プログラム                                          */
/*    C code written by K. Ando and K. Sekitani (Shizuoka Univ.)  */
/*                                                                */
/*              Version 2008.12.9                                 */
/*              Version 2013.05.07                                */
/*              Version 2013.05.22 revised                        */
/*              Version 2014.05.13 revised                        */
/*              Version 2015.06.17 revised                        */
/*                                                                */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define MAX_N 10000   // 点の数の最大値
#define INF 100000000 // 無限大の定義
#define EPSILON 0.00000001 //ε 小さい正の値
#define SWAP(a,b){int temp; temp=(a); (a)=(b); (b)=temp; }

struct point {
    int x;
    int y;
};

double dist(struct point p, struct point q) { // pとq の間の距離を計算
    return sqrt((p.x-q.x)*(p.x-q.x)+(p.y-q.y)*(p.y-q.y));
}

double tour_length(struct point p[MAX_N], int n, int tour[MAX_N]) {
    int i;
    double sum=0.0;
    for(i=0;i<n;i++) sum+=dist(p[tour[i]],p[tour[(i+1)%n]]);
    return sum;// 総距離が関数の戻り値
}

void read_tsp_data(char *filename, struct point p[MAX_N],int *np) {
    FILE *fp;
    char buff[100];
    int i;

    if ((fp=fopen(filename,"rt")) == NULL) {// 指定ファイルを読み込み用に開く
        fprintf(stderr,"Error: File %s open failed.\n",filename);
        exit(0);
    }

    while((fgets(buff,sizeof(buff),fp)!=NULL)   // DIMENSION で始まる行に出会う
    &&(strncmp("DIMENSION",buff,9)!=0)) ; // まで読み飛ばす.
    sscanf(buff,"DIMENSION : %d",np);           // 点の数 *np を読み込む

    while((fgets(buff,sizeof(buff),fp)!=NULL)   // NODE_COORD_SECTIONで始まる
    &&(strncmp("NODE_COORD_SECTION",buff,18)!=0)) ; // 行に出会うまで,
    // 読み飛ばす.
    for(i=0;i<*np;i++) {                       // i=0 から i=(*np)-1まで
        if(fgets(buff,sizeof(buff),fp)!=NULL)
        sscanf(buff,"%*d %d %d",&(p[i].x),&(p[i].y)); // i番目の点の座標を読み込む
    }

    fclose(fp);
}

void nn(int s, struct point p[MAX_N], int n, int tour[MAX_N]){
    FILE *fp;
    int i, j, nearest;
    int visited[MAX_N]; // 都市iが訪問済みならば1そうでなければ0
    double min;


    for(i = 0;i < n; i++) visited[i] = 0; // 最初は全都市は未訪問
    tour[0] = s;         // 最初に訪問する都市は 0
    visited[s] = 1;      // 都市0は訪問済み
    //printf("start %d\n", s);
    //デバッグ用

    for(i = 0; i < n - 1; i++) {
        //最後に訪問した都市tour[i]から最短距離にある未訪問都市nearestを
        //見つける
        min = 0;
        //printf("Loop : %d \n", i);
        //デバッグ用
        for(j = 0; j <= n; j++){

            if(!visited[j]  && (min == 0 || dist(p[tour[i]], p[j]) < min)){
                //printf("            visited[%d] : %d | min : %f | dist[%d][%d] : %f\n", j, visited[j], min, tour[i], j, dist(p[tour[i]], p[j]));
                //デバッグ用
                min = dist(p[tour[i]], p[j]);
                nearest = j;
            }
        }
        //printf("        nearest : %d \n",nearest);
        //デバッグ用
        tour[i + 1] = nearest; // i+1 番目に訪問する都市を nearest にして,
        visited[nearest] = 1;// nearest を訪問済みとする.
    }

}

void TwoOpt(struct point p[MAX_N], int n, int tour[MAX_N]){
    struct point a,b,c,d;
    int i,j,k,l,g,h,success;
    int count;
    //課題(ここから)
    do{
        count= 0;
        for(i = 0; i <= n - 3; i++){
            j = i + 1;
            for(k = i + 2; k <= n - 1; k++){
                l = (k + 1) % n;
                a = p[tour[i]]; b = p[tour[j]];
                c = p[tour[k]]; d = p[tour[l]];

                if(dist(a, b) + dist(c, d) - dist(a, c) - dist(b, d) > EPSILON){
                    g = j; h = k;
                    while (g < h) {
                        SWAP(tour[g], tour[h]);
                        g++;
                        h--;
                    }
                    count++;
                }
            }
        }
    }while(count > 0); //更新が行われなかった場合終了
}

void write_tour_data(char *filename, int n, int tour[MAX_N]){
    FILE *fp;
    int i;
    // 構築した巡回路をfilenameという名前のファイルに書き出すためにopen
    if((fp=fopen(filename,"wt"))==NULL){
        fprintf(stderr,"Error: File %s open failed.\n",filename);
        exit(EXIT_FAILURE);
    }
    fprintf(fp,"%d\n",n);
    for(i=0;i<n; i++){
        fprintf(fp,"%d ",tour[i]);
    }
    fprintf(fp,"\n");
    fclose(fp);
}

main(int argc, char *argv[]) {
    int  i, n;                   // 点の数
    struct point  p[MAX_N];   // 各点の座標を表す配列
    int tour[MAX_N];   // 巡回路を表現する配列
    double min;

    if(argc != 2) {
        fprintf(stderr,"Usage: %s <tsp_filename>\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    // 点の数と各点の座標を1番目のコマンドライン引数で指定されたファイルから読み込む
    read_tsp_data(argv[1],p, &n);
    // 最近近傍法による巡回路構築
    for(i = 0; i < n; i++){
        nn(i,p,n,tour);
        // 巡回路をテキストファイルとして出力
        printf("length : %lf\n\n", i, tour_length(p,n,tour));
        //デバッグ用
        if(!min || tour_length(p,n,tour) < min){
            write_tour_data("tour.dat",n,tour);
            min = tour_length(p,n,tour);
        }
    }
    // 巡回路をテキストファイルとして出力
    write_tour_data("tour1.dat",n,tour);
    // 巡回路長を画面に出力
    printf("%lf\n",tour_length(p,n,tour));
    // 2opt による改善
    TwoOpt(p,n,tour);
    // 巡回路をテキストファイルとして出力
    write_tour_data("tour2.dat",n,tour);
    // 巡回路長を画面に出力
    printf("%lf\n",tour_length(p,n,tour));

    exit(EXIT_SUCCESS);
}
