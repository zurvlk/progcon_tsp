#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
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

int decisionChange(double delta, double t){
    if(delta <= 0) return 1;
    //if(delta/t >= 1) return 0;
    if(((double)rand() / RAND_MAX) < exp(- delta / t)) return 1;
    return 0;
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

void sa(struct point p[MAX_N],
        int n, int tour[MAX_N],
        double initialT,
        double finalT,
        double coolingRate){

    int
        i, j, k, l, g, h;
    double
        t,
        currentTotalDistance,
        newTotalDistance;
    struct point a, b, c, d;
    currentTotalDistance = tour_length(p,n,tour);
    for(t = initialT; t > finalT; t *= coolingRate){
        for(i = 0; i <= n - 3; i++){
            j = i + 1;
            for(k = i + 2; k <= n - 1; k++){
                l = (k + 1) % n;
                a = p[tour[i]]; b = p[tour[j]];
                c = p[tour[k]]; d = p[tour[l]];
                //2-Opt近傍による新規経路長生成
                newTotalDistance = currentTotalDistance - (dist(a, b) + dist(c, d) - dist(a, c) - dist(b, d));
                //関数decisionChangeによって巡回路を適用させるか設定。
                if(decisionChange(newTotalDistance - currentTotalDistance, t)){
                    g = j; h = k;
                    while (g < h) {
                        SWAP(tour[g], tour[h]);
                        g++;
                        h--;
                    }
                    currentTotalDistance = newTotalDistance;
                }
            }
        }
    }
}

double buildRoute(struct point p[MAX_N], int n, int tour[MAX_N]){
    int i;
    for(i = 0; i < n; i++){
        tour[i] = i;
    }
    return tour_length(p,n,tour);
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

int main(int argc, char *argv[]) {
    int  n, i, j;
    int times = 5;
    struct point  p[MAX_N];   // 各点の座標を表す配列
    int tour[MAX_N];   // 巡回路を表現する配列

    double
        initialT = 1000.0,
        finalT = 0.1,
        coolingRate = 0.95,
        initalRoute,
        min = 0;

    if(argc != 2) {
        fprintf(stderr,"Usage: %s <tsp_filename>\n",argv[0]);
        exit(EXIT_FAILURE);
    }
    //乱数種生成
    srand((unsigned)time(NULL));
    // 点の数と各点の座標を1番目のコマンドライン引数で指定されたファイルから読み込む
    read_tsp_data(argv[1],p, &n);
    initalRoute = buildRoute(p,n,tour);
    min = initalRoute;
    // 巡回路をテキストファイルとして出力
    write_tour_data("init.dat",n,tour);
    write_tour_data("result.dat",n,tour);
    // 巡回路長を画面に出力
    printf("Intial Tour Length: %.2lf\n",tour_length(p,n,tour));
    for(i = 0; i < times; i++){
        //初期経路作成、Simulated Annealingによる巡回路の最適化
        buildRoute(p,n,tour);
        sa(p, n, tour, initialT, finalT, coolingRate);

        //出力関係
        printf("Degree of Progress %3.1lf%% |",100 * (double)i/(double)times);
        for(j = 0; j < ((double)i/(double)times)*20; j++) putchar('#');
        for(j = 0; j < 20 - ((double)i/(double)times)*20; j++) putchar(' ');
        if(!min || ( tour_length(p,n,tour) < min)){
            write_tour_data("result.dat",n,tour);
            min = tour_length(p,n,tour);
        }
        printf("| Temporarily Tour Length:%.2lf\r",min);
    }
    printf("Degree of Progress 100.0%%|");
    for(j = 0; j < 20; j++) putchar('#');
    printf("| Complete! Shortest Tour Length:%.2lf\n",min);

    exit(EXIT_SUCCESS);
}
