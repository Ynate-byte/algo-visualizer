#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Tăng giới hạn bộ nhớ để xử lý văn bản dài
#define MAX_NODES 1024 

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

// --- HÀM MỚI: Xử lý ký tự đặc biệt cho JSON ---
// Hàm này biến đổi ký tự xuống dòng thật thành chuỗi "\n" để JSON không bị lỗi
void print_escaped_string(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        unsigned char c = str[i];
        if (c == '\n') printf("\\n");      // Xử lý xuống dòng
        else if (c == '\r') printf("\\r");  // Xử lý về đầu dòng
        else if (c == '\t') printf("\\t");  // Xử lý phím Tab
        else if (c == '\"') printf("\\\""); // Xử lý dấu ngoặc kép
        else if (c == '\\') printf("\\\\"); // Xử lý dấu gạch chéo
        else if (c < 32) printf("?");       // Thay các ký tự điều khiển lạ bằng ?
        else printf("%c", c);               // Ký tự bình thường thì in ra
    }
}

void print_state(int step, HuffmanNode nodes[], int count, char* msg) {
    if (step > 0) printf(",\n");
    printf("{\"step\": %d, \"msg\": \"", step);
    // Escape msg để an toàn
    print_escaped_string(msg); 
    printf("\", \"nodes\": [");
    
    for(int i=0; i<count; i++) {
        printf("{\"id\": %d, \"name\": \"", nodes[i].id);
        
        // --- SỬA: Dùng hàm escape thay vì in trực tiếp %s ---
        print_escaped_string(nodes[i].name);
        // --------------------------------------------------
        
        printf("\", \"freq\": %d, \"active\": %d, \"left\": %d, \"right\": %d, \"is_new\": %d, \"code\": \"%s\"}", 
               nodes[i].freq, nodes[i].is_active, nodes[i].left, nodes[i].right, nodes[i].is_new, nodes[i].code);
        
        if(i < count - 1) printf(",");
    } 
    printf("]}");
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
    HuffmanNode nodes[MAX_NODES]; 
    int n=0;
    
    for(int i=0; i<MAX_NODES; i++) strcpy(nodes[i].code, "");

    if (argc == 3) {
        // Lưu ý: strtok cắt theo dấu phẩy, nên nếu văn bản có dấu phẩy sẽ bị cắt sai logic input.
        // Tuy nhiên ở đây ta tập trung sửa lỗi JSON trước.
        char *token_char = strtok(argv[1], ",");
        while (token_char != NULL) {
            nodes[n].id = n;
            strcpy(nodes[n].name, token_char);
            nodes[n].freq = 0; 
            nodes[n].is_active=1; nodes[n].left=-1; nodes[n].right=-1; nodes[n].is_new=0;
            n++;
            token_char = strtok(NULL, ",");
        }
        
        // Reset lại chuỗi thứ 2 để parse tần suất (Do strtok giữ state nội bộ, cần cẩn thận)
        // Trong trường hợp gọi từ Python, argv[2] là chuỗi riêng biệt nên an toàn.
        int i = 0;
        char *token_freq = strtok(argv[2], ",");
        while (token_freq != NULL && i < n) {
            nodes[i].freq = atoi(token_freq);
            i++;
            token_freq = strtok(NULL, ",");
        }
    } else {
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
        for(int k=0; k<total; k++) nodes[k].is_new=0;

        for(int k=0; k<total; k++) if(nodes[k].is_active) if(m1==-1||nodes[k].freq<nodes[m1].freq) m1=k;
        for(int k=0; k<total; k++) if(nodes[k].is_active && k!=m1) if(m2==-1||nodes[k].freq<nodes[m2].freq) m2=k;
        
        if(m1!=-1 && m2!=-1) {
            int newIdx=total; 
            
            if (newIdx >= MAX_NODES) {
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
            
            // Xử lý tên hiển thị khi gộp để tránh lỗi JSON nếu tên node con chứa ký tự lạ
            // Ở đây đơn giản hóa chỉ in SUM và tần suất để an toàn và ngắn gọn
            char msg[100]; 
            sprintf(msg, "Gộp node tần suất %d và %d", nodes[m1].freq, nodes[m2].freq);
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