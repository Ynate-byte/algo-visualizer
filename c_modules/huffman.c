#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { 
    int id; 
    char name[20]; 
    int freq; 
    int is_active; 
    int left; 
    int right; 
    int is_new; 
    char code[32];
} HuffmanNode;

void print_state(int step, HuffmanNode nodes[], int count, char* msg) {
    if (step > 0) printf(",\n");
    printf("{\"step\": %d, \"msg\": \"%s\", \"nodes\": [", step, msg);
    for(int i=0; i<count; i++) {
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
    HuffmanNode nodes[100]; int n=0;
    for(int i=0; i<100; i++) strcpy(nodes[i].code, "");

    if (argc == 3) {
        // --- SỬA LỖI STRTOK TẠI ĐÂY ---
        // Bước 1: Đọc danh sách ký tự trước
        char *token_char = strtok(argv[1], ",");
        while (token_char != NULL) {
            nodes[n].id = n;
            strcpy(nodes[n].name, token_char);
            nodes[n].is_active=1; nodes[n].left=-1; nodes[n].right=-1; nodes[n].is_new=0;
            n++;
            token_char = strtok(NULL, ",");
        }

        // Bước 2: Đọc danh sách tần suất sau
        int i = 0;
        char *token_freq = strtok(argv[2], ",");
        while (token_freq != NULL && i < n) {
            nodes[i].freq = atoi(token_freq);
            i++;
            token_freq = strtok(NULL, ",");
        }
    } else {
        // Dữ liệu mẫu nếu không có input
        char c[]={'a','b','c','d','e','f'}; int f[]={5,9,12,13,16,45}; n=6;
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
        for(int k=0; k<total; k++) nodes[k].is_new=0;

        for(int k=0; k<total; k++) if(nodes[k].is_active) if(m1==-1||nodes[k].freq<nodes[m1].freq) m1=k;
        for(int k=0; k<total; k++) if(nodes[k].is_active && k!=m1) if(m2==-1||nodes[k].freq<nodes[m2].freq) m2=k;
        
        if(m1!=-1 && m2!=-1) {
            int newIdx=total; nodes[newIdx].id=newIdx;
            sprintf(nodes[newIdx].name, "%d", nodes[m1].freq+nodes[m2].freq);
            nodes[newIdx].freq=nodes[m1].freq+nodes[m2].freq;
            nodes[newIdx].is_active=1; nodes[newIdx].left=m1; nodes[newIdx].right=m2; nodes[newIdx].is_new=1;
            strcpy(nodes[newIdx].code, ""); 

            nodes[m1].is_active=0; nodes[m2].is_active=0;
            total++; active_count--; 
            
            char msg[100]; sprintf(msg, "Gộp node %s (%d) và %s (%d)", nodes[m1].name, nodes[m1].freq, nodes[m2].name, nodes[m2].freq);
            print_state(step++, nodes, total, msg);
        } else break; 
    }

    int root = -1;
    for(int i=0; i<total; i++) if(nodes[i].is_active) { root = i; break; }
    if(root != -1) { char buffer[32]; generate_codes(nodes, root, buffer, 0); }

    print_state(step++, nodes, total, "Hoàn tất & Sinh mã bit."); 
    printf("]"); 
    return 0;
}