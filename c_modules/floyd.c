#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// SỬA 1: Tăng giá trị INF để tránh lỗi logic so sánh
#define INF 1000000000 
int N=4;

// THÊM: Hàm xử lý chuỗi cho JSON an toàn
void print_escaped_string(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        unsigned char c = str[i];
        if (c == '\"') printf("\\\"");
        else if (c == '\\') printf("\\\\");
        else printf("%c", c);
    }
}

void print_matrix(int step, int **dist, int k, int ci, int cj, int hi, int hj, char* msg) {
    if (step > 0) printf(",\n");
    // SỬA 2: Dùng print_escaped_string cho msg
    printf("{\"step\": %d, \"k\": %d, \"i\": %d, \"j\": %d, \"high_i\": %d, \"high_j\": %d, \"msg\": \"", step, k, ci, cj, hi, hj);
    print_escaped_string(msg);
    printf("\", \"matrix\": [");
    
    for(int i=0; i<N; i++) { 
        printf("["); 
        for(int j=0; j<N; j++) { 
            // Xử lý in ra JSON: nếu là INF thì in 999 hoặc số nào đó để JS hiển thị ký hiệu
            // Tuy nhiên frontend của bạn check 999 để hiển thị '∞'.
            // Vì ta đổi INF thành 1 tỷ, ta cần mapping lại khi in ra JSON để khớp với Frontend
            if (dist[i][j] == INF) printf("999"); 
            else printf("%d", dist[i][j]); 
            
            if(j<N-1) printf(","); 
        } 
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
        for(int i=0; i<N; i++) for(int j=0; j<N; j++) { 
            if(t){ 
                int val = atoi(t);
                // Nếu input là 999 (quy ước ∞ từ frontend), gán thành INF chuẩn của C
                dist[i][j] = (val == 999) ? INF : val; 
                t=strtok(NULL, ","); 
            } else dist[i][j]=(i==j)?0:INF; 
        }
    } else {
        // Data mẫu cũng phải sửa INF
        int temp[4][4]={{0,5,INF,10},{INF,0,3,INF},{INF,INF,0,1},{INF,INF,INF,0}};
        for(int i=0; i<N; i++) for(int j=0; j<N; j++) dist[i][j] = (i<4&&j<4)?temp[i][j]:((i==j)?0:INF);
    }
    
    printf("["); print_matrix(step++, dist, -1, -1, -1, -1, -1, "Khởi tạo ma trận.");
    
    for (int k=0; k<N; k++) {
        for (int i=0; i<N; i++) {
            for (int j=0; j<N; j++) {
                // Logic Floyd-Warshall chuẩn: Tránh cộng INF
                if (dist[i][k] != INF && dist[k][j] != INF) {
                    if (dist[i][k] + dist[k][j] < dist[i][j]) {
                        dist[i][j] = dist[i][k] + dist[k][j];
                        char msg[100]; 
                        // Format msg an toàn
                        sprintf(msg, "Cập nhật dist[%d][%d]=%d qua trung gian %d", i, j, dist[i][j], k);
                        print_matrix(step++, dist, k, i, j, i, j, msg);
                    }
                }
            }
        }
    }
    
    print_matrix(step++, dist, N, -1, -1, -1, -1, "Hoàn tất."); printf("]");
    for(int i=0;i<N;i++) free(dist[i]); free(dist);
    return 0;
}