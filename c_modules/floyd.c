#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// INF 1 tỷ để tính toán an toàn
#define INF 1000000000 
#define INPUT_INF 999 

int N = 4;

// HÀM MỚI: Xử lý chuỗi an toàn cho JSON
void print_escaped_string(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        unsigned char c = str[i];
        if (c == '\"') printf("\\\"");
        else if (c == '\\') printf("\\\\");
        else printf("%c", c);
    }
}

// HÀM MỚI: Tái tạo đường đi từ mảng next[][] thành chuỗi "A -> B -> C"
void get_path_string(char* buffer, int u, int v, int **next) {
    if (next[u][v] == -1) {
        sprintf(buffer, "Không có đường đi");
        return;
    }
    
    // Bắt đầu từ đỉnh u
    sprintf(buffer, "%c", 65 + u);
    
    int curr = u;
    // Lặp để tìm các đỉnh tiếp theo
    while (curr != v) {
        curr = next[curr][v];
        char temp[10];
        sprintf(temp, " -> %c", 65 + curr);
        strcat(buffer, temp);
        
        // Tránh lặp vô tận nếu có lỗi logic (an toàn)
        if (strlen(buffer) > 200) break; 
    }
}

void print_matrix(int step, int **dist, int k, int ci, int cj, int hi, int hj, char* msg) {
    if (step > 0) printf(",\n");
    
    printf("{\"step\": %d, \"k\": %d, \"i\": %d, \"j\": %d, \"high_i\": %d, \"high_j\": %d, \"msg\": \"", step, k, ci, cj, hi, hj);
    print_escaped_string(msg);
    printf("\", \"matrix\": [");
    
    for(int i=0; i<N; i++) { 
        printf("["); 
        for(int j=0; j<N; j++) { 
            // Map về 999 cho Frontend
            if (dist[i][j] >= INF/2) printf("999"); 
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
    int **next = (int **)malloc(N * sizeof(int *)); // Mảng truy vết đường đi
    
    for(int i=0; i<N; i++) {
        dist[i] = (int *)malloc(N * sizeof(int));
        next[i] = (int *)malloc(N * sizeof(int));
    }
    
    int step=0;
    
    // 1. INPUT VÀ KHỞI TẠO
    if(argc==3) {
        char *t = strtok(argv[2], ",");
        for(int i=0; i<N; i++) for(int j=0; j<N; j++) { 
            if(t){ 
                int val = atoi(t);
                if (val >= INPUT_INF) {
                    dist[i][j] = INF;
                    next[i][j] = -1; // Không có đường đi
                } else {
                    dist[i][j] = val;
                    // Nếu có cạnh và i khác j, đỉnh tiếp theo của i là j
                    next[i][j] = (i != j) ? j : i; 
                }
                t=strtok(NULL, ","); 
            } else {
                dist[i][j]=(i==j)?0:INF;
                next[i][j] = (i==j)?i:-1;
            }
        }
    } else {
        // Dữ liệu mẫu
        int temp[4][4]={{0,5,INF,10},{INF,0,3,INF},{INF,INF,0,1},{INF,INF,INF,0}};
        for(int i=0; i<N; i++) for(int j=0; j<N; j++) {
            dist[i][j] = (i<4&&j<4)?temp[i][j]:((i==j)?0:INF);
            if (dist[i][j] != INF && i != j) next[i][j] = j;
            else if (i == j) next[i][j] = i;
            else next[i][j] = -1;
        }
    }
    
    printf("["); 
    print_matrix(step++, dist, -1, -1, -1, -1, -1, "Khởi tạo ma trận khoảng cách và ma trận truy vết (Next).");
    
    // 2. THUẬT TOÁN FLOYD-WARSHALL
    for (int k=0; k<N; k++) {
        for (int i=0; i<N; i++) {
            for (int j=0; j<N; j++) {
                
                // Kiểm tra đường đi qua K có tồn tại không
                if (dist[i][k] != INF && dist[k][j] != INF) {
                    
                    if (dist[i][k] + dist[k][j] < dist[i][j]) {
                        dist[i][j] = dist[i][k] + dist[k][j];
                        
                        // CẬP NHẬT TRUY VẾT 
                        // Để đi từ i đến j qua k, bước tiếp theo từ i sẽ giống như bước tiếp theo để đi từ i đến k
                        next[i][j] = next[i][k]; 
                        
                        // Tạo thông báo chi tiết bao gồm LỘ TRÌNH MỚI
                        char path_str[256];
                        get_path_string(path_str, i, j, next);
                        
                        char msg[512]; 
                        sprintf(msg, "Tìm thấy đường ngắn hơn qua %c (%d + %d = %d). Lộ trình mới: %s", 
                                65+k, dist[i][k], dist[k][j], dist[i][j], path_str);
                        
                        print_matrix(step++, dist, k, i, j, i, j, msg);
                    }
                }
            }
        }
    }
    
    // 3. KIỂM TRA CHU TRÌNH ÂM 
    int has_negative_cycle = 0;
    for(int i=0; i<N; i++) {
        if(dist[i][i] < 0) {
            has_negative_cycle = 1;
            char msg[100];
            sprintf(msg, "CẢNH BÁO: Phát hiện chu trình âm tại đỉnh %c (Cost: %d)!", 65+i, dist[i][i]);
            print_matrix(step++, dist, -1, i, i, i, i, msg);
        }
    }

    if (!has_negative_cycle) {
        print_matrix(step++, dist, N, -1, -1, -1, -1, "Hoàn tất. Không phát hiện chu trình âm."); 
    }
    
    printf("]");
    
    // Giải phóng bộ nhớ
    for(int i=0;i<N;i++) { free(dist[i]); free(next[i]); }
    free(dist); free(next);
    
    return 0;
}