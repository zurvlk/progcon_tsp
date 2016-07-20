/*                                                                */
/*    訪問順制約付きTSP用プログラム                                    */
/*    C code written by K. Ando and K. Sekitani (Shizuoka Univ.)  */
/*                                                                */
/*                                                                */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <omp.h>

#define MAX_N 10000   // 点の数の最大値
#define INF 100000000 // 無限大の定義
#define EPSILON 0.00000001 //ε 小さい正の値
#define SWAP(a,b){int stemp; stemp=(a); (a)=(b); (b)=stemp; }

struct point {
    int x;
    int y;
};

float dist(struct point p, struct point q) { // pとq の間の距離を計算
    return sqrt((p.x - q.x)*(p.x - q.x) + (p.y - q.y)*(p.y - q.y));
}

float tour_length(struct point p[MAX_N], int n, int tour[MAX_N]) {
    int i;
    float sum = 0.0;
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

int countPrec(int start, int end, int tour[MAX_N], int m, int prec[MAX_N]){
    int i, j, count;
    for(i = start; i <= end; i++){
        for(j = 0; j < m; j++){
            if(tour[i] == prec[j]) count++;
        }
    }
    return count;
}

int chkPrec(int j, int m, int prec[MAX_N],int visited[MAX_N]){
    int i, k;
    for(i = 0; i < m; i++){

        if(j == prec[i]){
            for(k = 0; k < i; k++){
                if(visited[prec[k]] == 0){
                    return 0;
                }
            }
            return 1;
        }
    }
    return 1;
}

int chkRoute(int n, int tour[MAX_N], int m, int prec[MAX_N], int precLocation[MAX_N]){
    int i,
        errCount = 0,
        precCount = 0,
        visited[n];

    for(i = 0; i < n; i++){
        if(tour[i] == prec[precCount]){
            precCount++;
            precLocation[precCount] = i;
        }
        visited[tour[i]] = 1;
    }

    if(precCount != m){
        errCount++;
    }

    for(i = 0; i < n; i++){
         if(!visited[i]){
             errCount++;
         }
     }
     return errCount;
}

int decisionChange(float delta, float t){
    if(delta <= 0) return 1;
    if(((float)rand() / RAND_MAX) < exp(-delta / t)) return 1;
    return 0;
}

void CI(struct point p[MAX_N],
	int n,
	int tour[MAX_N],
	int m,
	int prec[MAX_N],
    int precLocation[MAX_N]){
	FILE *fp;

	int i, j, k, x, nearest, count,
	   visited[MAX_N],
       save[MAX_N];
	float min;

	for(i = 0; i < n; i++) tour[i] = 0; visited[i] = 0;
	//prec内の点をすべてtour内に格納する
	for(i=0; i < m; i++){
		tour[i] = prec[i];
		visited[prec[i]] = 1; // 都市iは訪問済み
	}

	count = m;
	while(count < n){
		for(i = 0; i < n; i++){ save[i] = 0;}
		min = INF;
		for(i = 0; i < count - 1; i++) {
			j = (i + 1) % count;
			for(k = 0; k < n; k++){
				if( !visited[k] && dist(p[tour[i]], p[k]) + dist(p[tour[j]], p[k]) - dist(p[tour[i]], p[tour[j]])< min ){
						min = dist(p[tour[i]], p[k]) + dist(p[tour[j]], p[k]) - dist(p[tour[i]], p[tour[j]]);
						nearest = k;
						x = i;
				}
			}
		}

		for(i = x; i < n; i++) save[i+1] = tour[i+1];
		tour[x + 1] = nearest;
		visited[nearest] = 1;
		for(i = x; i < n; i++) tour[i+2] = save[i+1];
		count++;
	}

	for(i = 0; i < n; i++) printf("%d ",tour[i]);
	putchar('\n');
	chkRoute(n, tour, m, prec, precLocation);
}

void twoOpt(struct point p[MAX_N],
        int n, int tour[MAX_N],
        int head, int tail,
        float initialT,
        float finalT,
        float coolingRate){
    int
        g, h, i, j, k, l;
    float
        t,
        currentTotalDistance,
        newTotalDistance;
    struct point a, b, c, d;

    currentTotalDistance = tour_length(p,n,tour);

    for(t = initialT; t > finalT; t *= coolingRate){
        for(i = head; i <= tail - 3; i++){
            j = i + 1;
            for(k = i + 2; k <= tail - 1; k++){
                l = (k + 1) % n;
                a = p[tour[i]]; b = p[tour[j]];
                c = p[tour[k]]; d = p[tour[l]];

                newTotalDistance = currentTotalDistance - (dist(a, b) + dist(c, d) - dist(a, c) - dist(b, d));

                if(newTotalDistance / currentTotalDistance <= 5 && decisionChange(newTotalDistance - currentTotalDistance, t)){

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

void orOpt(struct point p[MAX_N],
        int n, int tour[MAX_N],
        int head, int tail){

    int i, j, k, l, m, end, success,
        a0 = -1, b0 = -1, c0 = -1, d0 = -1, e0 = -1;
    float len[2], max, pastlen;
    struct point a, b, c, d, e;

    do{
        end = 0;
        max = EPSILON;
        for(i = 0; i <= n - 4; i++){
            j = i + 1;
            for(k = i + 3; k <= n - 2; k++){
                l = (k + 1) % n;
                m = (k - 1) % n;

                a = p[tour[i]]; b = p[tour[j]];
                c = p[tour[k]]; d = p[tour[l]];
                e = p[tour[m]];

                len[0] = dist(a, b) + dist(e, c) + dist(c, d) - dist(a, c) - dist(c, b) - dist(e, d);

                if(len[0] > EPSILON && i != l && j != l){
                    success = 1;
                    end = 1;
                    if(len[0] > max){
                        max = len[0];
                        a0 = i; b0 = j; c0 = k; d0 = l; e0 = m;
                    }
                }
            }
        }

        pastlen = tour_length(p, n, tour);
        if(end == 1){
            while(c0 > b0){
                SWAP(tour[c0],tour[c0 - 1]);
                c0--;
            }
        }
    }while(!end);
}

void Improvement(struct point p[MAX_N],
    int n, int tour[MAX_N],
    int m, int prec[MAX_N],
    float initialT,
    float finalT,
    float coolingRate){

    int i, j, k;

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

    struct point p[MAX_N];   // 各点の座標を表す配列
    int n;                   // 点の数
    int m;                   // 順序制約に現れる点の数
    int initTour[MAX_N],
        tour[MAX_N],
        prec[MAX_N],
        precLocation[MAX_N],
        i, j,
        times = 40,
        count = 0;
    float
        initialT = 1000.0,
        finalT = 0.01,
        coolingRate = 0.95,
        min = 0;

    if(argc != 2) {
        fprintf(stderr,"Usage: %s <tsp_filename>\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    // 点の数と各点の座標を1番目のコマンドライン引数で指定されたファイルから読み込む
    read_tsp_data(argv[1], p, &n, prec, &m);
    CI(p, n, initTour, m, prec, precLocation);

    srand((unsigned)time(NULL));

    printf("This program use %d Threads\n",omp_get_max_threads());
    printf("init tour length (CI): %.3lf\n\n", tour_length(p,n,initTour));

    #pragma omp parallel for private(tour, j)

    for(i = 0; i < 1; i++){

        for(j = 0; j < n; j++) tour[j] = initTour[j];
        Improvement(p, n, tour, m, prec, initialT, finalT, coolingRate);

        if(!min || ( tour_length(p,n,tour) < min)){
            write_tour_data("res_sar.dat",n,tour);
            min = tour_length(p,n,tour);
            count++;
            if(count < 10) putchar('0');
            printf("%d : %.3lf *\n", count,tour_length(p,n,tour));
        }else{
            if(count < 10) putchar('0');
            count++;
            printf("%d : %.3lf\n", count,tour_length(p,n,tour));
        }

    }

    exit(EXIT_SUCCESS);
}
