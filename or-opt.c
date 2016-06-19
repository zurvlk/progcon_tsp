/* 巡回セールスマン問題を解く局所探索法による近似
アルゴリズム（2-OPTおよびOR-OPT近傍) */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define MAX_N 1000         //点の数の最大
#define INF 100000000
#define ZERO 0.000001   /* 許容誤差幅 */
#define SEED 256       /* 乱数の種 */
struct point          /* 構造体pの定義 */
{
    int x;
    int y;
};
int n;                /* 外部変数：点の個数 */
struct point p[MAX_N];    /* p[i].xとp[i].yは点iの座標 */

void read_tsp_data(char *filename, struct point p[MAX_N],int *np);
double dist(struct point p, struct point q);
void nn(struct point p[MAX_N], int n, int tour[MAX_N]);
double local(int *init, double lginit, int *lopt);
double length(int i, int j, int *x);
int rand_from(int min, int max);

main(int argc, char *argv[]){
    int i, init[MAX_N], tour[MAX_N];;
    double initlg, bestlg;

    if(argc != 2) {
        fprintf(stderr,"Usage: %s <tsp_filename>\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    read_tsp_data(argv[1],p, &n);
    nn(p,n,init);
    initlg = tour_length(p,n,init);
    srand(SEED);
    write_tour_data("tour1.dat",n,init); // 巡回路長を画面に出力
    printf("%lf\n",initlg);

    bestlg=local(init, initlg, tour);  /* 局所探索 */
    printf("local optimal tour\n");    /* 結果の出力 */
}

double dist(struct point p, struct point q) { // pとq の間の距離を計算
    return sqrt((p.x-q.x)*(p.x-q.x)+(p.y-q.y)*(p.y-q.y));
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

void nn(struct point p[MAX_N], int n, int init[MAX_N]){
    FILE *fp;
    int i,j,nearest;
    int visited[MAX_N]; // 都市iが訪問済みならば1そうでなければ0
    int min;

    for(i=0;i<n;i++) visited[i]=0; // 最初は全都市は未訪問
    init[0]=0;         // 最初に訪問する都市は 0
    visited[0]=1;      // 都市0は訪問済み

    for(i=0;i<n-1;i++) {
        //最後に訪問した都市tour[i]から最短距離にある未訪問都市nearestを
        //見つける
        min = INF;
        for(j=1;j<n;j++) {
            if(!visited[j] && dist(p[init[i]],p[j])<min){
                nearest=j;      //都市tour[i]から暫定的に最短距離にある未訪問都市をnearestとする
                min = dist(p[init[i]],p[j]); // その暫定最短距離をminとする
            }
        }
        init[i+1]=nearest; // i+1 番目に訪問する都市を nearest にして,
        visited[nearest]=1;// nearest を訪問済みとする.
    }
}


double local(int *init, double initlg, int *lopt){
    int i, i0, j, k, h, temp, tm[3];
    float lg, lgtemp, lg0, lg1;
    for(i=0; i<n; i++) lopt[i]=init[i];
    lg=initlg;
    i0=0;
    RESTART:                             /* 近傍探索の開始 */
    for(i=i0; i<i0+n; i++)               /* 2-OPT近傍の探索 */
    {
        for(j=i+2; j<i+n-1; j++)            /* lgtempは長さの増分 */
        {
            lgtemp=length(i, j, lopt)+length(i+1, j+1, lopt)
            -length(i, i+1, lopt)-length(j, j+1, lopt);
            if(lgtemp<-ZERO)                   /* 改良解の発見 */
            {
                lg=lg+lgtemp;
                for(k=0; k<(j-i)/2; k++)          /* 改良解の構成 */
                {
                    temp=lopt[(i+1+k)%n]; lopt[(i+1+k)%n]=lopt[(j-k)%n];
                    lopt[(j-k)%n]=temp;
                }
                printf("improved solution by 2-OPT\n");
                goto IMPROVED;
            }
        }
    }
    for(i=i0; i<i0+n; i++)             /* Or-OPT近傍の探索 */
    {
        for(k=i+1; k<=i+3; k++)
        {
            for(j=k+1; j<i+n-1; j++)         /* lgtempは長さの増分 */
            {
                lg0=length(i, i+1, lopt)+length(j, j+1,lopt)+length(k, k+1, lopt);
                lg1=length(i, k+1, lopt)+length(j, k, lopt)+length(i+1, j+1,lopt);
                lgtemp=lg1-lg0;
                if(lgtemp<-ZERO)                /* 改良解の発見 */
                {
                    lg=lg+lgtemp;                  /* 改良解の構成 */
                    for(h=i+1; h<=k; h++) tm[h-i-1]=lopt[h%n];
                    for(h=k+1; h<=j; h++) lopt[(h-k+i)%n]=lopt[h%n];
                    for(h=0; h<k-i; h++) lopt[(j-k+i+1+h)%n]=tm[k-i-1-h];
                    printf("improved solution by Or-OPT\n");
                    goto IMPROVED;
                }
                if(k==i+1) continue;
                lg1=length(i, k+1, lopt)+length(j, i+1, lopt) /* 逆方向の挿入 */
                +length(k, j+1, lopt);
                lgtemp=lg1-lg0;
                if(lgtemp<-ZERO)                /* 改良解の発見 */
                {
                    lg=lg+lgtemp;                  /* 改良解の構成 */
                    for(h=i+1; h<=k; h++) tm[h-i-1]=lopt[h%n];
                    for(h=k+1; h<=j; h++) lopt[(h-k+i)%n]=lopt[h%n];
                    for(h=0; h<k-i; h++) lopt[(j-h)%n]=tm[k-i-1-h];
                    printf("improved solution by Or-OPT\n");
                    goto IMPROVED;
                }
            }
        }
    }
    return(lg);                     /* 局所探索終了 */
    IMPROVED:                       /* 暫定解の更新：次の近傍探索へ */
    goto RESTART;
}

double length(int i, int j, int *x){
    return(sqrt(pow(p[x[i%n]].x-p[x[j%n]].x, 2)
    +pow(p[x[i%n]].y-p[x[j%n]].y, 2)));
}

int rand_from(int min, int max){
    return((int)(((double)rand()/((unsigned)RAND_MAX+1))
    *(max-min+1))+min);
}
