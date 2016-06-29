/*              SimulatedAnnealing(SA)を用いた巡回路構築プログラム              */
/* gcc4.4.7,gcc6.1.1で動作確認済み、コンパイル時に-lm -fopenmpオプションが必要です. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <omp.h>
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
double ThreeOpt(struct point p[MAX_N], int n, int tour[MAX_N]){
    struct point a, b, c, d, e, f;
    int i, j, k, l, v, w, z, success,
        g1, g2, g3, h1, h2, h3, num;
	double dmax, max, initDistance, len[7];


	success = 1;
    initDistance = tour_length(p,n,tour);

	while(success != 0){
		success = 0;
		max = EPSILON;
		for(i = 0; i <= n - 5; i++){
			j = i+1;
			for(k = i + 2; k <= n - 3; k++){
				l = (k + 1)%n;
				for(v = k + 2; v <= n - 1; v++){
					w = (v + 1)%n;
					a = p[tour[i]]; d = p[tour[j]];
					e = p[tour[k]]; b = p[tour[l]];
					c = p[tour[v]]; f = p[tour[w]];

					len[0] = dist(a,d) + dist(e,b) + dist(c,f) - dist(a,c) - dist(b,d) - dist(e,f);
					len[1] = dist(a,d) + dist(e,b) + dist(c,f) - dist(a,b) - dist(c,e) - dist(d,f);
					len[2] = dist(a,d) + dist(e,b) + dist(c,f) - dist(a,b) - dist(c,d) - dist(e,f);
					len[3] = dist(a,d) + dist(e,b) + dist(c,f) - dist(a,e) - dist(d,c) - dist(b,f);
					len[4] = dist(a,d) + dist(b,e) - dist(a,e) - dist(b,d);
					len[5] = dist(b,e) + dist(c,f) - dist(b,f) - dist(c,e);
					len[6] = dist(a,d) + dist(c,f) - dist(a,c) - dist(d,f);

					dmax = EPSILON;
					for(z = 0; z <= 6; z++){
						if( dmax < len[z] && len[z] > EPSILON) {
							dmax = len[z];
							num = z;
							success = 1;
						}
					}
					if (dmax > EPSILON && i != w){
						if(dmax > max){
							max = dmax;

							switch (num) {
								case 0:
									g1 = j; h1 = v;
									g2 = v-(k-j); h2 = v;
									g3 = -1; h3 = -2;
									break;

								case 1:
									g1 = j; h1 = v;
									g2 = j; h2 = j+(v-l);
									g3 = -1; h3 = -2;
									break;

								case 2:
									g1 = j; h1 = v;
									g2 = j; h2 = j+(v-l);
									g3 = v-(k-j); h3 = v;
									break;

								case 3:
									g1 = j; h1 = k;
									g2 = l; h2 = v;
									g3 = -1; h3 = -2;
									break;

								case 4:
									g1 = j; h1 = k;
									g2 = -1; h2 = -2;
									g3 = -1; h3 = -2;
									break;

								case 5:
									g1 = l; h1 = v;
									g2 = -1; h2 = -2;
									g3 = -1; h3 = -2;
									break;

								case 6:
									g1 = j; h1 = v;
									g2 = -1; h2 = -2;
									g3 = -1; h3 = -2;
									break;
							}
						}
					}
				}
			}
		}

		while(g1<h1){
			SWAP(tour[g1],tour[h1]);
			g1++; h1--;
		}

		while(g2<h2){
			SWAP(tour[g2],tour[h2]);
			g2++; h2--;
		}

		while(g3<h3){
			SWAP(tour[g3],tour[h3]);
			g3++; h3--;
		}
	}
    return initDistance - tour_length(p,n,tour);
}

double TwoOpt(struct point p[MAX_N], int n, int tour[MAX_N]){
    struct point a, b, c, d;
    int i, j, k, l, g, h, success, count;
    double initDistance;
    initDistance = tour_length(p,n,tour);
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

    return initDistance - tour_length(p,n,tour);
}

double OrOpt(struct point p[MAX_N], int n, int tour[MAX_N]){
    struct point a, b, c, d, e;
    int i, j, k, l, m, g, h, success, end,
        a0 = -1, b0 = -1, c0 = -1, d0 = -1, e0 = -1;
	double len[2], max, pastlen, newlen, initDistance;

    initDistance = tour_length(p, n, tour);
	success = 1;
	while(success){
		success = 0;

		end = 1;
		while(end){
			end = 0;
			max = EPSILON;
			for(i = 0; i <= n - 4; i++){
				j = i + 1;
				for(k = i + 3; k <= n - 2; k++){
					l = (k + 1)%n;
					m = (k - 1)%n;

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

		}
	}

    return initDistance - tour_length(p,n,tour);
}

void sa(struct point p[MAX_N],
        int n, int tour[MAX_N],
        int times,
        double initialT,
        double finalT,
        double coolingRate){

    int
        i, j, k, l, g, h,
        noChangeCounts = 0;
    double
        t,
        pastTotalDistance,
        currentTotalDistance,
        newTotalDistance;
    struct point a, b, c, d;

    currentTotalDistance = tour_length(p,n,tour);
    pastTotalDistance = currentTotalDistance;

    for(t = initialT; t > finalT; t *= coolingRate){
        for(i = 0; i <= n - 3; i++){
            j = i + 1;
            for(k = i + 2; k <= n - 1; k++){
                l = (k + 1) % n;
                a = p[tour[i]]; b = p[tour[j]];
                c = p[tour[k]]; d = p[tour[l]];

                newTotalDistance = currentTotalDistance - (dist(a, b) + dist(c, d) - dist(a, c) - dist(b, d));

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

        if(pastTotalDistance == currentTotalDistance) noChangeCounts++;
        else noChangeCounts = 0;

        if(noChangeCounts == 50) return;
    }
}

void buildRoute(int n, int tour[MAX_N]){
    int i;
    for(i = 0; i < n; i++){
        tour[i] = i;
    }
}

void buildRandRoute(int n, int tour[MAX_N]){
    int i, randKey, numRand;
    int temp[MAX_N];
    buildRoute(n,temp);
    numRand = n;

    for(i = 0; i < n; i++){
        randKey = rand() % numRand;
        tour[i] = temp[randKey];
        temp[randKey] = temp[numRand - 1];

        --numRand;
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
    int
        n, i = 0,
        count = 0,
        cores,
        times = 10;
    struct point  p[MAX_N];   // 各点の座標を表す配列
    int tour[MAX_N];
    double
        initialT = 1000.0,
        finalT = 0.1,
        coolingRate = 0.99,
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
    buildRandRoute(n,tour);
    // 巡回路をテキストファイルとして出力
    write_tour_data("tour1.dat",n,tour);
    // 巡回路長を画面に出力
    //
    cores = omp_get_max_threads();
    printf("This program use %d cores\n",cores);


    #pragma omp parallel for private(tour)

    for(i = 0; i < times * cores;i++){
        buildRandRoute(n,tour);

        sa(p, n, tour, times, initialT, finalT, coolingRate);
        while ((TwoOpt(p, n, tour) != 0 && ThreeOpt(p, n, tour) != 0) && OrOpt(p, n, tour) != 0);

        count++;
        if(count < 10) putchar('0');
        printf("%d : %.3lf ", count,tour_length(p,n,tour));

        if(!min || ( tour_length(p,n,tour) < min)){
            write_tour_data("res_sa.dat",n,tour);
            min = tour_length(p,n,tour);
            putchar('*');
        }
        putchar('\n');
    }
    // 巡回路長を画面に出力
    printf("Result: %s --> %lf\n", argv[1], min);

    exit(EXIT_SUCCESS);
}
