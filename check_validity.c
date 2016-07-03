#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#define INF 100000000 // 無限大の定義
#define MAX_N 10000
#define INTERVAL 30

#define PROBLEM_DIR "./in_data2"

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

void read_tsp_data(char *filename, struct point p[MAX_N],int *np, int prec[MAX_N], int *mp) {
  FILE *fp;
  char buff[500];
  int i;

  if ((fp=fopen(filename,"rt")) == NULL) {// 指定ファイルを読み込み用に開く
    fprintf(stderr,"Error: File %s open failed.\n",filename);
    exit(EXIT_FAILURE);
  }   

  while((fgets(buff,sizeof(buff),fp)!=NULL)   // PRECEDENCE_CONSTRAINTS:で始まる行に出会う
	&&(strncmp("PRECEDENCE_CONSTRAINTS:",buff,23)!=0)) ; // まで読み飛ばす. 
  if(strncmp("PRECEDENCE_CONSTRAINTS:",buff,23)==0)  {
    sscanf(buff+24,"%d",mp);
    for(i=0;i<*mp;i++) fscanf(fp,"%d ",&prec[i]);
  } else {
    fprintf(stderr,"Error: There is no precedence constraint in file %s.\n",filename);
    exit(EXIT_FAILURE);
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

void copyarray(int dst[MAX_N], int src[MAX_N], int n) {
  int i;
  for(i=0;i<n;i++) dst[i]=src[i];
}

void shiftarray(int k, int arr[MAX_N], int n) {
  int i, tmp[MAX_N];
  copyarray(tmp,arr,n);
  for (i=0;i<n;i++) arr[(n+i-k)%n]=tmp[i];
}

void printarray(int arr[], int n) {
  int i;

  for(i=0;i<n;i++) printf("%d ",arr[i]);
  printf("\n");
}

int check_validity(int tour[MAX_N], int n, int prec[MAX_N], int m) {
  int i,j,start=-1;
  int check[MAX_N];
  int order[MAX_N];

  for(i=0;i<n;i++) check[i]=0;
  for(j=0;j<m;j++) order[j]=0;

  for(i=0;i<n;i++) check[tour[i]]+=1; //tour に含まれている都市は何個あるか計数

  for(i=0;i<n;i++) 
    if(check[i]!=1) {
      printf("Error: %d appears %d times.\n",i,check[i]);
      return 0;
    }

  for(i=0;start==-1 && i<n;i++) {
    for(j=0;j<m;j++) {
      if(tour[i]==prec[j]) {
	start=j;
	break;
      }
    }
  }

  shiftarray(start,prec,m);
  //printarray(prec,m);
  for(j=0;j<m;j++) 
    for(i=0;i<n;i++) {
      if(tour[i]==prec[j]) order[j]=i;
    }

  for(j=1;j<m;j++) 
    if(order[j-1]>order[j]) {
      printf("Error: Order constraint %d < %d is violated.\n",prec[j-1],prec[j]);
      return 0;
   }

  return 1;
}

int read_tour_data(char *filename, int tour[MAX_N], int n){
  FILE *fp;
  int i,nn;

  if ((fp=fopen(filename,"rt")) == NULL) {
    printf("Error: file %s does not exits.\n",filename);
       return 0;
  }
  fscanf(fp,"%d",&nn);
  if(nn!=n) {
    printf("Error: Number of cities in tsp file is %d while that in the tour is %d.\n",n,nn);
       return 0;
  }
  for(i=0; i<n; i++) fscanf(fp,"%d",&tour[i]);
  fclose(fp);

  return 1;
}

main(int argc, char *argv[]) {
  FILE *fp;
  double tl;
  int i,j;
  char filename[60];
  char dat_filename[60];
  char tsp_filename[60];
  int n;
  int m;
  int filenamelength;
  int tour[MAX_N];
  int prec[MAX_N];
  struct point p[MAX_N];

  if(argc!=2) {
    fprintf(stderr,"Usage: %s <dat filename>\n",argv[0]);
    exit(EXIT_FAILURE);
  }

  filenamelength=strlen(argv[1]);

  if(strncmp(argv[1]+(filenamelength-4),".dat",4)) {
    fprintf(stderr,"Filename must end with dat.\n");
    exit(EXIT_FAILURE);
  }

  strcpy(dat_filename,argv[1]);
  strncpy(filename,dat_filename,(filenamelength-4));
  filename[filenamelength-4]='\0';

  sprintf(tsp_filename,"%s/%s.tsp",PROBLEM_DIR,filename);

  read_tsp_data(tsp_filename, p,&n,prec,&m);

  
  if(read_tour_data(dat_filename, tour,n)&&check_validity(tour,n,prec,m)) {
    printf("Valid dat file. ");
    printf("%lf\n",tour_length(p,n,tour));
  }


}


