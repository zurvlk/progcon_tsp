#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
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
    if(delta/t >= 1) return 0;
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

void nn(struct point p[MAX_N], int n, int tour[MAX_N]){
    FILE *fp;
    int i,j,nearest;
    int visited[MAX_N]; // 都市iが訪問済みならば1そうでなければ0
    int min;

    for(i=0;i<n;i++) visited[i]=0; // 最初は全都市は未訪問
    tour[0]=0;         // 最初に訪問する都市は 0
    visited[0]=1;      // 都市0は訪問済み

    for(i=0;i<n-1;i++) {
        //最後に訪問した都市tour[i]から最短距離にある未訪問都市nearestを
        //見つける
        min = INF;
        for(j=1;j<n;j++) {
            if(!visited[j] && dist(p[tour[i]],p[j])<min){
                nearest=j;      //都市tour[i]から暫定的に最短距離にある未訪問都市をnearestとする
                min = dist(p[tour[i]],p[j]); // その暫定最短距離をminとする
            }
        }
        tour[i+1]=nearest; // i+1 番目に訪問する都市を nearest にして,
        visited[nearest]=1;// nearest を訪問済みとする.
    }

}

void TwoOpt(struct point p[MAX_N], int n, int tour[MAX_N]){
    struct point a,b,c,d;
    int i,j,k,l,g,h,success;

    //課題(ここから)
    success = 0;
    for(i = 0; i <= n - 3; i++){
        j = i + 1;
        for(k = i + 2; k <= n - 1; k++){
            l = (k + 1) % n;
            a = p[tour[i]]; b = p[tour[j]];
            c = p[tour[k]]; d = p[tour[l]];

            if(dist(a, b) + dist(c, d) - dist(a, c) - dist(b, d) > EPSILON){
                success = 1;
                g = j; h = k;
                while (g < h) {
                    SWAP(tour[g], tour[h]);
                    g++;
                    h--;
                }
                if(success) return;
            }
        }
    }
}

void sa(struct point p[MAX_N],
        int n, int tour[MAX_N],
        int times,
        double initialT,
        double finalT,
        double coolingRate){

    int
        randomIndex1,
        randomIndex2,
        i,j,k,l,g,h,success;
    double
        t,
        currentTotalDistance,
        newTotalDistance;
    struct point a,b,c,d;
    currentTotalDistance = tour_length(p,n,tour);

    for(t = initialT; t > finalT; t *= coolingRate){
        for(i = 0; i < times; i++){
            randomIndex1 = rand() % n;
            randomIndex2 = rand() % n;

            SWAP(tour[randomIndex1], tour[randomIndex2]);
            newTotalDistance = tour_length(p,n,tour);

            if(decisionChange(newTotalDistance - currentTotalDistance, t))
                currentTotalDistance = newTotalDistance;
            else
                SWAP(tour[randomIndex1], tour[randomIndex2]);
        }
    }
}

void buildRoute(struct point p[MAX_N], int n, int tour[MAX_N]){
    int i;
    for(i = 0; i < n; i++){
        tour[i] = i;
    }
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
    int  n, i = 0;
    int times = 1000;
    struct point  p[MAX_N];   // 各点の座標を表す配列
    int tour[MAX_N];   // 巡回路を表現する配列

    double
        initialT = 1000.0,
        finalT = 0.1,
        coolingRate = 0.95,
        min = 0;

    if(argc != 2) {
        fprintf(stderr,"Usage: %s <tsp_filename>\n",argv[0]);
        exit(EXIT_FAILURE);
    }
    //乱数種生成
    srand((unsigned)time(NULL));
    // 点の数と各点の座標を1番目のコマンドライン引数で指定されたファイルから読み込む
    read_tsp_data(argv[1],p, &n);
    // 最近近傍法による巡回路構築
    buildRoute(p,n,tour);
    // 巡回路をテキストファイルとして出力
    write_tour_data("tour1.dat",n,tour);
    // 巡回路長を画面に出力
    printf("%lf\n",tour_length(p,n,tour));
    for(i = 0; i < 30;i++){
        buildRoute(p,n,tour);
        sa(p, n, tour, times, initialT, finalT, coolingRate);
        printf("%lf\n",tour_length(p,n,tour));
        if(!min || ( tour_length(p,n,tour) < min)){
            write_tour_data("tour2.dat",n,tour);
            min = tour_length(p,n,tour);
        }
    }



    // 巡回路長を画面に出力
    printf("%lf\n",min);

    exit(EXIT_SUCCESS);
}
