#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INF 999
int N=4;

void print_matrix(int step, int **dist, int k, int ci, int cj, int hi, int hj, char* msg) {
    if (step > 0) printf(",\n");
    printf("{\"step\": %d, \"k\": %d, \"i\": %d, \"j\": %d, \"high_i\": %d, \"high_j\": %d, \"msg\": \"%s\", \"matrix\": [", step, k, ci, cj, hi, hj, msg);
    for(int i=0; i<N; i++) { 
        printf("["); 
        for(int j=0; j<N; j++) { printf("%d", dist[i][j]); if(j<N-1) printf(","); } 
        printf("]"); 
        if(i<N-1) printf(","); 
    } 
    printf("]}");
}

int main(int argc, char *argv[]) {
    if(argc>=2) N=atoi(argv[1]);
    int **dist = (int **)malloc(N * sizeof(int *));
    for(int i=0; i<N; i++) dist[i] = (int *)malloc(N * sizeof(int));
    int step=0;
    if(argc==3) {
        char *t = strtok(argv[2], ",");
        for(int i=0; i<N; i++) for(int j=0; j<N; j++) { if(t){ dist[i][j]=atoi(t); t=strtok(NULL, ","); } else dist[i][j]=(i==j)?0:INF; }
    } else {
        int temp[4][4]={{0,5,INF,10},{INF,0,3,INF},{INF,INF,0,1},{INF,INF,INF,0}};
        for(int i=0; i<N; i++) for(int j=0; j<N; j++) dist[i][j] = (i<4&&j<4)?temp[i][j]:((i==j)?0:INF);
    }
    printf("["); print_matrix(step++, dist, -1, -1, -1, -1, -1, "Khởi tạo ma trận.");
    for (int k=0; k<N; k++) for (int i=0; i<N; i++) for (int j=0; j<N; j++) {
        if (dist[i][k]!=INF && dist[k][j]!=INF && dist[i][k]+dist[k][j] < dist[i][j]) {
            dist[i][j] = dist[i][k]+dist[k][j];
            char msg[100]; sprintf(msg, "Cập nhật dist[%d][%d]=%d", i, j, dist[i][j]);
            print_matrix(step++, dist, k, i, j, i, j, msg);
        }
    }
    print_matrix(step++, dist, N, -1, -1, -1, -1, "Hoàn tất."); printf("]");
    for(int i=0;i<N;i++) free(dist[i]); free(dist);
    return 0;
}