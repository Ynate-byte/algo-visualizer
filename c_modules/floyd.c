#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// SỬA 1: Tăng INF lên 1 tỷ để tránh lỗi logic khi đường đi dài > 999
#define INF 1000000000 
#define INPUT_INF 999 // Giá trị quy ước từ Frontend gửi xuống

int N=4;

// Hàm xử lý chuỗi an toàn cho JSON (Copy từ huffman/binary sang)
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
            // Khi in ra JSON cho Frontend hiển thị, cần chuyển INF về lại 999 (hoặc ký hiệu vô cực)
            // để khớp với logic hiển thị của Web
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
    // Input format: floyd [size] [matrix_string]
    if(argc>=2) N=atoi(argv[1]);
    
    int **dist = (int **)malloc(N * sizeof(int *));
    for(int i=0; i<N; i++) dist[i] = (int *)malloc(N * sizeof(int));
    
    int step=0;
    
    if(argc==3) {
        char *t = strtok(argv[2], ",");
        for(int i=0; i<N; i++) for(int j=0; j<N; j++) { 
            if(t){ 
                int val = atoi(t);
                // SỬA 3: Map giá trị 999 từ input thành INF chuẩn của C
                dist[i][j] = (val >= INPUT_INF) ? INF : val; 
                t=strtok(NULL, ","); 
            } else {
                dist[i][j]=(i==j)?0:INF; 
            }
        }
    } else {
        // Dữ liệu mẫu (Hardcode)
        int temp[4][4]={{0,5,INF,10},{INF,0,3,INF},{INF,INF,0,1},{INF,INF,INF,0}};
        for(int i=0; i<N; i++) for(int j=0; j<N; j++) dist[i][j] = (i<4&&j<4)?temp[i][j]:((i==j)?0:INF);
    }
    
    printf("["); 
    print_matrix(step++, dist, -1, -1, -1, -1, -1, "Khởi tạo ma trận khoảng cách.");
    
    // Thuật toán Floyd-Warshall
    for (int k=0; k<N; k++) {
        for (int i=0; i<N; i++) {
            for (int j=0; j<N; j++) {
                // Kiểm tra quan trọng: Không cộng INF (tránh tràn số hoặc logic sai)
                if (dist[i][k] != INF && dist[k][j] != INF) {
                    if (dist[i][k] + dist[k][j] < dist[i][j]) {
                        dist[i][j] = dist[i][k] + dist[k][j];
                        
                        char msg[200]; 
                        sprintf(msg, "Cập nhật đường đi từ %c đến %c qua trung gian %c: %d", 
                                65+i, 65+j, 65+k, dist[i][j]); // 65 là mã ASCII của 'A'
                        
                        print_matrix(step++, dist, k, i, j, i, j, msg);
                    }
                }
            }
        }
    }
    
    print_matrix(step++, dist, N, -1, -1, -1, -1, "Hoàn tất thuật toán."); 
    printf("]");
    
    // Giải phóng bộ nhớ
    for(int i=0;i<N;i++) free(dist[i]); 
    free(dist);
    
    return 0;
}