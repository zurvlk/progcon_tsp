#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_N 10000   // 点の数の最大値
#define INF 100000000 // 無限大の定義
#define EPSILON 0.00000001 //ε 小さい正の値
#define INDIVIDUAL 100 //個体数
#define SWAP(a,b){int stemp; stemp=(a); (a)=(b); (b)=stemp; }

int population[INDIVIDUAL][MAX_N];

struct point {
    int x;
    int y;
};

float dist(struct point p, struct point q) {
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
        visited[n],
        temporaryPrecLocation[MAX_N];

    for(i = 0; i < n; i++){
        if(tour[i] == prec[precCount]){
            precCount++;
            temporaryPrecLocation[precCount] = i;
        }
        visited[tour[i]] = 1;
    }

    if(precCount != m){
        errCount++;
        return 1;
    }

    for(i = 0; i < n; i++){
         if(!visited[i]){
             errCount++;
             return 2;
         }
     }

     if(!errCount)
        for(i = 0; i < m; i++) precLocation[i] = temporaryPrecLocation[i];

     return 0;
}

int decisionChange(float delta, float t){
    if(delta <= 0) return 1;
    //if(((float)rand() / RAND_MAX) < exp(-delta / t)) return 1;
    return 0;
}

void FCI(struct point p[MAX_N],
	int n,
	int tour[MAX_N],
	int m,
	int prec[MAX_N],
    int precLocation[MAX_N]){
	FILE *fp;
	int i, j, k, farthest, count, x, y,
	   visited[MAX_N], save[MAX_N]; // 都市iが訪問済みならば1そうでなければ0
	float min,max;


	for(i = 0; i < n; i++) { tour[i] =0; visited[i] = 0; }// 最初は全都市は未訪問

	//prec内の点をすべてtour内に格納する
	for(i = 0; i < m; i++){
		tour[i] = prec[i];
		visited[prec[i]] = 1; // 都市iは訪問済み
	}

	//閉路の中から２点選んでその２点からもっとも遠い点を選ぶ
	count = m;

	while(count < n){
		max = -1;
		for(i = 0; i < count; i++) {
			j=(i + 1)%count;
			for(k = 0; k < n; k++){
				if( !visited[k] && dist(p[tour[i]], p[k]) + dist(p[tour[j]], p[k]) - dist(p[tour[i]], p[tour[j]]) > max ){
					max = dist(p[tour[i]], p[k]) + dist(p[tour[j]], p[k]) - dist(p[tour[i]], p[tour[j]]);
					farthest = k;
					x = i;
				}
			}
		}

		y = farthest;
		min = INF;
		for(i=0; i < count; i++){
			if( dist(p[tour[i]],p[y]) +dist(p[tour[(i + 1)%count]],p[y]) -dist(p[tour[i]],p[tour[(i + 1)%count]]) < min ){
				min = dist(p[tour[i]],p[y]) +dist(p[tour[(i + 1)%count]],p[y]) -dist(p[tour[i]],p[tour[(i + 1)%count]]);
				x = i;
			}
		}

		for(i = x; i < count; i++) {save[i] = tour[i + 1];}
		tour[x + 1] = farthest; // i+1 番目に訪問する都市を farthest にして,
		visited[farthest] = 1;// farthest を訪問済みとする.
		for(i = x; i < count; i++) {tour[i + 2] = save[i];}
		count++;
	}

	for(i = 0; i < n; i++) printf("%d ",tour[i]);
		putchar('\n');
	if(!chkRoute(n, tour, m, prec, precLocation)) printf("initRoute : OK\n");
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
    int n;
    int m;
    int
        prec[MAX_N],
        i, j,
        genMax = 10000,
        individuals = 100,
        gen = 0;

    float
        min = 0;


    if(argc != 2) {
        fprintf(stderr,"Usage: %s <tsp_filename>\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    read_tsp_data(argv[1], p, &n, prec, &m);
    FCI(p, n, population[0], m, prec, precLocation);

    srand((unsigned)time(NULL));
    exit(EXIT_SUCCESS);
}
