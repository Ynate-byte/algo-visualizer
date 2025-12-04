#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- SỬA 1: Tăng giới hạn Node lên 1024 ---
#define MAX_NODES 1024 

typedef struct { 
    int id; 
    char name[20]; // Tên ký tự
    int freq; 
    int is_active; 
    int left; 
    int right; 
    int is_new; 
    char code[32]; // Chuỗi bit mã hóa
} HuffmanNode;

// (Giữ nguyên hàm print_state và generate_codes không đổi...)
void print_state(int step, HuffmanNode nodes[], int count, char* msg) {
    if (step > 0) printf(",\n");
    printf("{\"step\": %d, \"msg\": \"%s\", \"nodes\": [", step, msg);
    for(int i=0; i<count; i++) {
        // In node JSON...
        printf("{\"id\": %d, \"name\": \"%s\", \"freq\": %d, \"active\": %d, \"left\": %d, \"right\": %d, \"is_new\": %d, \"code\": \"%s\"}", 
               nodes[i].id, nodes[i].name, nodes[i].freq, nodes[i].is_active, nodes[i].left, nodes[i].right, nodes[i].is_new, nodes[i].code);
        if(i < count - 1) printf(",");
    } printf("]}");
}

void generate_codes(HuffmanNode nodes[], int currIndex, char tempCode[], int depth) {
    if (currIndex == -1) return;
    if (nodes[currIndex].left == -1 && nodes[currIndex].right == -1) {
        tempCode[depth] = '\0'; strcpy(nodes[currIndex].code, tempCode); return;
    }
    tempCode[depth] = '0'; generate_codes(nodes, nodes[currIndex].left, tempCode, depth + 1);
    tempCode[depth] = '1'; generate_codes(nodes, nodes[currIndex].right, tempCode, depth + 1);
}

int main(int argc, char *argv[]) {
    // --- SỬA 2: Sử dụng MAX_NODES thay vì 100 ---
    HuffmanNode nodes[MAX_NODES]; 
    int n=0;
    
    // Reset code
    for(int i=0; i<MAX_NODES; i++) strcpy(nodes[i].code, "");

    if (argc == 3) {
        char *token_char = strtok(argv[1], ",");
        while (token_char != NULL) {
            nodes[n].id = n;
            strcpy(nodes[n].name, token_char);
            nodes[n].freq = 0; 
            nodes[n].is_active=1; nodes[n].left=-1; nodes[n].right=-1; nodes[n].is_new=0;
            n++;
            token_char = strtok(NULL, ",");
        }
        
        int i = 0;
        char *token_freq = strtok(argv[2], ",");
        while (token_freq != NULL && i < n) {
            nodes[i].freq = atoi(token_freq);
            i++;
            token_freq = strtok(NULL, ",");
        }
    } else {
        // Dữ liệu mẫu
        char c[]={'A','B','C','D'}; int f[]={4, 2, 1, 1}; n=4;
        for(int i=0;i<n;i++){
            nodes[i].id=i; sprintf(nodes[i].name,"%c",c[i]); nodes[i].freq=f[i]; 
            nodes[i].is_active=1; nodes[i].left=-1; nodes[i].right=-1; nodes[i].is_new=0;
        }
    }

    int total=n, step=0; 
    printf("["); print_state(step++, nodes, total, "Khởi tạo các node lá.");

    int active_count = n;
    while(active_count > 1) {
        int m1=-1, m2=-1;
        // Reset cờ is_new
        for(int k=0; k<total; k++) nodes[k].is_new=0;

        // Tìm 2 node nhỏ nhất
        for(int k=0; k<total; k++) if(nodes[k].is_active) if(m1==-1||nodes[k].freq<nodes[m1].freq) m1=k;
        for(int k=0; k<total; k++) if(nodes[k].is_active && k!=m1) if(m2==-1||nodes[k].freq<nodes[m2].freq) m2=k;
        
        if(m1!=-1 && m2!=-1) {
            int newIdx=total; 
            
            // --- SỬA 3: Kiểm tra tràn bộ nhớ ---
            if (newIdx >= MAX_NODES) {
                 // Nếu tràn, dừng ngay để tránh crash, in ra state hiện tại rồi thoát
                 char err[50]; sprintf(err, "LỖI: Tràn bộ nhớ (>%d nodes)", MAX_NODES);
                 print_state(step++, nodes, total, err);
                 break; 
            }

            nodes[newIdx].id=newIdx;
            strcpy(nodes[newIdx].name, "SUM"); 
            nodes[newIdx].freq=nodes[m1].freq+nodes[m2].freq;
            nodes[newIdx].is_active=1; nodes[newIdx].left=m1; nodes[newIdx].right=m2; nodes[newIdx].is_new=1;
            strcpy(nodes[newIdx].code, ""); 

            nodes[m1].is_active=0; nodes[m2].is_active=0;
            total++; active_count--; 
            
            char msg[100]; sprintf(msg, "Gộp %s(%d) và %s(%d) -> SUM(%d)", nodes[m1].name, nodes[m1].freq, nodes[m2].name, nodes[m2].freq, nodes[newIdx].freq);
            print_state(step++, nodes, total, msg);
        } else break; 
    }

    int root = -1;
    for(int i=0; i<total; i++) if(nodes[i].is_active) { root = i; break; }
    if(root != -1) { char buffer[32]; generate_codes(nodes, root, buffer, 0); }

    print_state(step++, nodes, total, "Hoàn tất."); 
    printf("]"); 
    return 0;
}