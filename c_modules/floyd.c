#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INF 1000000000 
#define INPUT_INF 999 

int N = 4;

// Hàm xử lý chuỗi an toàn cho JSON
void print_escaped_string(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        unsigned char c = str[i];
        if (c == '\"') printf("\\\"");
        else if (c == '\\') printf("\\\\");
        else printf("%c", c);
    }
}

// Hàm in JSON mới: Hỗ trợ mảng so sánh (attempts)
void print_row_step(int step, int **dist, int k, int i, int *attempts, int *updates, char* msg) {
    if (step > 0) printf(",\n");
    
    printf("{\"step\": %d, \"k\": %d, \"i\": %d, \"msg\": \"", step, k, i);
    print_escaped_string(msg);
    printf("\", \"matrix\": [");
    
    // In ma trận hiện tại
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
    printf("], \"attempts\": ["); // Mảng chứa các giá trị thử nghiệm cho hàng i
    
    // In mảng attempts (chỉ cho hàng i hiện tại)
    for(int c=0; c<N; c++) {
        if (attempts[c] >= INF/2) printf("999");
        else printf("%d", attempts[c]);
        if(c<N-1) printf(",");
    }
    
    printf("], \"updates\": ["); // Mảng đánh dấu ô nào sẽ được update (0/1)
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
    
    // Cấp phát bộ nhớ cho mảng tạm tính toán hàng
    int *row_attempts = (int *)malloc(N * sizeof(int));
    int *row_updates = (int *)malloc(N * sizeof(int));
    
    for(int i=0; i<N; i++) {
        dist[i] = (int *)malloc(N * sizeof(int));
        next[i] = (int *)malloc(N * sizeof(int));
    }
    
    int step=0;
    
    // 1. INPUT (Giữ nguyên như cũ)
    if(argc==3) {
        char *t = strtok(argv[2], ",");
        for(int i=0; i<N; i++) for(int j=0; j<N; j++) { 
            if(t){ 
                int val = atoi(t);
                if (val >= INPUT_INF) { dist[i][j] = INF; next[i][j] = -1; } 
                else { dist[i][j] = val; next[i][j] = (i != j) ? j : i; }
                t=strtok(NULL, ","); 
            } else {
                dist[i][j]=(i==j)?0:INF; next[i][j] = (i==j)?i:-1;
            }
        }
    } else {
        // Dữ liệu mẫu
        int temp[4][4]={{0,5,INF,10},{INF,0,3,INF},{INF,INF,0,1},{INF,INF,INF,0}};
        for(int i=0; i<N; i++) for(int j=0; j<N; j++) {
            dist[i][j] = (i<4&&j<4)?temp[i][j]:((i==j)?0:INF);
            next[i][j] = (dist[i][j]!=INF && i!=j) ? j : -1;
            if (i==j) next[i][j] = i;
        }
    }
    
    // Khởi tạo mảng so sánh rỗng
    for(int c=0; c<N; c++) { row_attempts[c] = -1; row_updates[c] = 0; }
    
    printf("["); 
    print_row_step(step++, dist, -1, -1, row_attempts, row_updates, "Khởi tạo ma trận.");
    
    // 2. THUẬT TOÁN FLOYD-WARSHALL (LOGIC MỚI: BATCH THEO HÀNG)
    for (int k=0; k<N; k++) {
        for (int i=0; i<N; i++) {
            
            // Nếu không thể đi từ i đến k, thì không cần xét hàng này với k
            if (dist[i][k] == INF) continue;

            int update_count = 0;
            
            // TÍNH TOÁN TRƯỚC (Pre-calculation)
            for (int j=0; j<N; j++) {
                row_attempts[j] = -1; // -1 nghĩa là không xét (do k->j là INF)
                row_updates[j] = 0;
                
                if (dist[k][j] != INF) {
                    int new_dist = dist[i][k] + dist[k][j];
                    row_attempts[j] = new_dist; // Lưu giá trị thử nghiệm
                    
                    if (new_dist < dist[i][j]) {
                        row_updates[j] = 1; // Đánh dấu sẽ update
                        update_count++;
                    }
                }
            }
            
            // CHỈ IN RA NẾU CÓ ÍT NHẤT 1 SỰ THAY ĐỔI HOẶC LÀ HÀNG QUAN TRỌNG
            // Để gọn, ta in ra mọi bước xét hàng i đi qua k (để thấy cả so sánh thất bại)
            char msg[200];
            if (update_count > 0) 
                sprintf(msg, "Đỉnh K=%c: Xét hàng %c. Có %d đường đi ngắn hơn được tìm thấy.", 65+k, 65+i, update_count);
            else
                sprintf(msg, "Đỉnh K=%c: Xét hàng %c. Không tìm thấy đường nào ngắn hơn.", 65+k, 65+i);

            print_row_step(step++, dist, k, i, row_attempts, row_updates, msg);
            
            // CẬP NHẬT CHÍNH THỨC
            for (int j=0; j<N; j++) {
                if (row_updates[j]) {
                    dist[i][j] = row_attempts[j];
                    next[i][j] = next[i][k];
                }
            }
        }
    }
    
    // Reset mảng tạm để in bước cuối
    for(int c=0; c<N; c++) { row_attempts[c] = -1; row_updates[c] = 0; }
    print_row_step(step++, dist, N, -1, row_attempts, row_updates, "Hoàn tất thuật toán.");
    
    printf("]");
    
    // Giải phóng
    for(int i=0;i<N;i++) { free(dist[i]); free(next[i]); }
    free(dist); free(next); free(row_attempts); free(row_updates);
    
    return 0;
}