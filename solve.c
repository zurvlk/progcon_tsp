/*                                                                */
/*    訪問順制約付きTSP用プログラム                                    */
/*    C code written by K. Ando and K. Sekitani (Shizuoka Univ.)  */
/*                                                                */
/*                                                                */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define MAX_N 10000   // 点の数の最大値
#define INF 100000000 // 無限大の定義
#define EPSILON 0.00000001 //ε 小さい正の値
#define SWAP(a,b){int temp; temp=(a); (a)=(b); (b)=temp; }

struct point {
    int x;
    int y;
};

double dist(struct point p, struct point q) { // pとq の間の距離を計算
    return sqrt((p.x - q.x)*(p.x - q.x) + (p.y - q.y)*(p.y - q.y));
}

double tour_length(struct point p[MAX_N], int n, int tour[MAX_N]) {
    int i;
    double sum = 0.0;
    for(i = 0; i < n; i++) sum += dist(p[tour[i]], p[tour[(i + 1)%n]]);
    return sum;// 総距離が関数の戻り値
}

void read_tsp_data(char *filename, struct point p[MAX_N],int *np, int prec[MAX_N], int *mp) {
    FILE *fp;
    char buff[500];
    int i;

    if ((fp = fopen(filename,"rt")) == NULL) {// 指定ファイルを読み込み用に開く
        fprintf(stderr, "Error: File %s open failed.\n", filename);
        exit(EXIT_FAILURE);
    }

    while((fgets(buff, sizeof(buff), fp) != NULL)   // PRECEDENCE_CONSTRAINTS:で始まる行に出会う
    &&(strncmp("PRECEDENCE_CONSTRAINTS:", buff, 23) != 0)) ; // まで読み飛ばす.
    if(strncmp("PRECEDENCE_CONSTRAINTS:", buff, 23) == 0)  {
        sscanf(buff + 24, "%d", mp);
        for(i =0 ;i < *mp; i++) fscanf(fp, "%d ", &prec[i]);
    } else {
        fprintf(stderr, "Error: There is no precedence constraint in file %s.\n", filename);
        exit(EXIT_FAILURE);
    }

    while((fgets(buff, sizeof(buff), fp) != NULL)   // DIMENSION で始まる行に出会う
    &&(strncmp("DIMENSION", buff, 9) != 0)) ; // まで読み飛ばす.
    sscanf(buff, "DIMENSION : %d", np);           // 点の数 *np を読み込む

    while((fgets(buff,sizeof(buff), fp) != NULL)   // NODE_COORD_SECTIONで始まる
    &&(strncmp("NODE_COORD_SECTION", buff, 18) != 0)) ; // 行に出会うまで,
    // 読み飛ばす.
    for(i = 0; i < *np; i++) {                       // i=0 から i=(*np)-1まで
        if(fgets(buff, sizeof(buff), fp) != NULL)
        sscanf(buff, "%*d %d %d", &(p[i].x), &(p[i].y)); // i番目の点の座標を読み込む
    }

    fclose(fp);
}

int chkPrec(int j, int m, int prec[MAX_N],int visited[MAX_N]){
    int i, k;
    for(i = 0; i < m; i++){

        if(j == prec[i]){
            printf("   --> %d : prec[%d]", j, i);
            for(k = 0; k < i; k++){
                if(visited[prec[k]] == 0){
                    printf(" NG\n");
                    return 0;
                }
            }
            printf(" OK\n");
            return 1;
        }
    }
    return 1;
}

void nn(struct point p[MAX_N],
        int n,
        int tour[MAX_N],
        int m,
        int prec[MAX_N]){

    FILE *fp;
    int i, j, nearest;
    int visited[MAX_N]; // 都市iが訪問済みならば1そうでなければ0
    double min;


    for(i = 0;i < n; i++) visited[i] = 0; // 最初は全都市は未訪問
    tour[0] = prec[0];
    visited[prec[0]] = 1;      // 都市0は訪問済み
    printf("1   start : %d\n", prec[0]);


    for(i = 0; i < n - 1; i++) {
        min = 0;
        for(j = 0; j <= n; j++){
            if(!visited[j]  && (min == 0 || dist(p[tour[i]], p[j]) < min)){
                //デバッグ用
                if(chkPrec(j, m, prec, visited)){
                    min = dist(p[tour[i]], p[j]);
                    nearest = j;
                }
            }
        }
        printf("\n%d nearest : %d \n", i + 2, nearest);
        //デバッグ用
        tour[i + 1] = nearest; // i+1 番目に訪問する都市を nearest にして,
        visited[nearest] = 1;// nearest を訪問済みとする.
    }

}

void write_tour_data(char *filename, int n, int tour[MAX_N]){
    FILE *fp;
    int i;

    // 構築した巡回路をfilenameという名前のファイルに書き出すためにopen
    if((fp = fopen(filename, "wt")) == NULL){
        fprintf(stderr, "Error: File %s open failed.\n", filename);
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "%d\n", n);
    for(i = 0; i < n; i++){
        fprintf(fp, "%d ", tour[i]);
    }
    fprintf(fp, "\n");
    fclose(fp);
}

int main(int argc, char *argv[]) {
    int n;                   // 点の数
    int m;                   // 順序制約に現れる点の数
    struct point p[MAX_N];   // 各点の座標を表す配列
    int tour[MAX_N];   // 巡回路を表現する配列
    int prec[MAX_N];   // 順序制約を表現する配列
    int i;

    if(argc != 2) {
        fprintf(stderr,"Usage: %s <tsp_filename>\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    // 点の数と各点の座標を1番目のコマンドライン引数で指定されたファイルから読み込む
    read_tsp_data(argv[1],p,&n,prec,&m);

    //順序制約の確認

    // 最近近傍法による巡回路構築
    nn(p, n, tour, m, prec);

    // ファイルに出力
    write_tour_data("tour1.dat", n, tour);
    printf("%5.1lf\n", tour_length(p, n, tour)); // 巡回路tourの長さ

    exit(EXIT_SUCCESS);
}
