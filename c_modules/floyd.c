#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INF 1000000000 
#define INPUT_INF 999 

int N = 4;

void print_escaped_string(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        unsigned char c = str[i];
        if (c == '\"') printf("\\\"");
        else if (c == '\\') printf("\\\\");
        else printf("%c", c);
    }
}

// CẬP NHẬT: Thêm tham số next (ma trận đường đi) vào JSON
void print_row_step(int step, int **dist, int **next, int k, int i, int *attempts, int *updates, char* msg) {
    if (step > 0) printf(",\n");
    
    printf("{\"step\": %d, \"k\": %d, \"i\": %d, \"msg\": \"", step, k, i);
    print_escaped_string(msg);
    printf("\", \"matrix\": [");
    
    // In ma trận khoảng cách
    for(int r=0; r<N; r++) { 
        printf("["); 
        for(int c=0; c<N; c++) { 
            if (dist[r][c] >= INF/2) printf("999"); 
            else printf("%d", dist[r][c]); 
            if(c<N-1) printf(","); 
        } 
        printf("]"); 
        if(r<N-1) printf(","); 
    } 
    
    // THÊM MỚI: In ma trận đường đi (next matrix)
    printf("], \"next_matrix\": [");
    for(int r=0; r<N; r++) { 
        printf("["); 
        for(int c=0; c<N; c++) { 
            printf("%d", next[r][c]); // Gửi số nguyên (index đỉnh), Frontend sẽ tự đổi ra A, B, C
            if(c<N-1) printf(","); 
        } 
        printf("]"); 
        if(r<N-1) printf(","); 
    }

    printf("], \"attempts\": ["); 
    for(int c=0; c<N; c++) {
        if (attempts[c] >= INF/2) printf("999");
        else printf("%d", attempts[c]);
        if(c<N-1) printf(",");
    }
    
    printf("], \"updates\": ["); 
    for(int c=0; c<N; c++) {
        printf("%d", updates[c]);
        if(c<N-1) printf(",");
    }
    printf("]}");
}

int main(int argc, char *argv[]) {
    if(argc>=2) N=atoi(argv[1]);
    
    int **dist = (int **)malloc(N * sizeof(int *));
    int **next = (int **)malloc(N * sizeof(int *));
    
    int *row_attempts = (int *)malloc(N * sizeof(int));
    int *row_updates = (int *)malloc(N * sizeof(int));
    
    for(int i=0; i<N; i++) {
        dist[i] = (int *)malloc(N * sizeof(int));
        next[i] = (int *)malloc(N * sizeof(int));
    }
    
    int step=0;
    
    // INPUT
    if(argc==3) {
        char *t = strtok(argv[2], ",");
        for(int i=0; i<N; i++) for(int j=0; j<N; j++) { 
            if(t){ 
                int val = atoi(t);
                if (val >= INPUT_INF) { dist[i][j] = INF; next[i][j] = -1; } 
                else { dist[i][j] = val; next[i][j] = (i != j) ? j : -1; } // Sửa logic next
                if (i==j) { dist[i][j]=0; next[i][j]=i; } // Chính nó
                t=strtok(NULL, ","); 
            } else {
                dist[i][j]=(i==j)?0:INF; next[i][j] = (i==j)?i:-1;
            }
        }
    } else {
        // Demo data
        int temp[4][4]={{0,5,INF,10},{INF,0,3,INF},{INF,INF,0,1},{INF,INF,INF,0}};
        for(int i=0; i<N; i++) for(int j=0; j<N; j++) {
            dist[i][j] = (i<4&&j<4)?temp[i][j]:((i==j)?0:INF);
            if(dist[i][j] == INF) next[i][j] = -1;
            else next[i][j] = (i!=j) ? j : i;
        }
    }
    
    for(int c=0; c<N; c++) { row_attempts[c] = -1; row_updates[c] = 0; }
    
    printf("["); 
    // Gửi bước 0 kèm cả dist và next
    print_row_step(step++, dist, next, -1, -1, row_attempts, row_updates, "Khởi tạo ma trận.");
    
    // FLOYD-WARSHALL
    for (int k=0; k<N; k++) {
        for (int i=0; i<N; i++) {
            if (dist[i][k] == INF) continue;

            int update_count = 0;
            for (int j=0; j<N; j++) {
                row_attempts[j] = -1; 
                row_updates[j] = 0;
                
                if (dist[k][j] != INF) {
                    int new_dist = dist[i][k] + dist[k][j];
                    row_attempts[j] = new_dist; 
                    if (new_dist < dist[i][j]) {
                        row_updates[j] = 1; 
                        update_count++;
                    }
                }
            }
            
            char msg[200];
            if (update_count > 0) sprintf(msg, "Đỉnh K=%c: Xét hàng %c. Cập nhật %d đường đi.", 65+k, 65+i, update_count);
            else sprintf(msg, "Đỉnh K=%c: Xét hàng %c. Không có tối ưu mới.", 65+k, 65+i);

            print_row_step(step++, dist, next, k, i, row_attempts, row_updates, msg);
            
            for (int j=0; j<N; j++) {
                if (row_updates[j]) {
                    dist[i][j] = row_attempts[j];
                    next[i][j] = next[i][k]; // Cập nhật hướng đi
                }
            }
        }
    }
    
    for(int c=0; c<N; c++) { row_attempts[c] = -1; row_updates[c] = 0; }
    print_row_step(step++, dist, next, N, -1, row_attempts, row_updates, "Hoàn tất thuật toán.");
    
    printf("]");
    
    for(int i=0;i<N;i++) { free(dist[i]); free(next[i]); }
    free(dist); free(next); free(row_attempts); free(row_updates);
    
    return 0;
}