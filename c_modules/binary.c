#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 100

// HÀM MỚI: Xử lý chuỗi an toàn cho JSON (copy từ huffman.c sang)
void print_escaped_string(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        unsigned char c = str[i];
        if (c == '\"') printf("\\\"");
        else if (c == '\\') printf("\\\\");
        else printf("%c", c);
    }
}

void print_json_array(int arr[], int n) {
    printf("["); 
    for(int i=0; i<n; i++) { 
        printf("%d", arr[i]); 
        if(i<n-1) printf(", "); 
    } 
    printf("]");
}

// Sửa lại hàm này để dùng print_escaped_string
void print_state(int step, char* msg, int arr[], int n, int key, int low, int high, int mid, int found) {
    if (step > 0) printf(",\n");
    printf("{\"step\": %d, \"msg\": \"", step);
    print_escaped_string(msg); // <--- In msg an toàn
    printf("\", \"key\": %d, \"low\": %d, \"high\": %d, \"mid\": %d, \"found\": %d", key, low, high, mid, found);
    
    if (step == 0) { 
        printf(", \"data\": "); 
        print_json_array(arr, n); 
    }
    printf("}");
}

int main(int argc, char *argv[]) {
    int arr[MAX_SIZE];
    // Khởi tạo mảng bằng 0 để tránh rác bộ nhớ
    memset(arr, 0, sizeof(arr)); 
    int n=0, key=0;

    if (argc == 4) {
        key = atoi(argv[1]); 
        n = atoi(argv[2]);
        
        // AN TOÀN: Giới hạn n để không tràn mảng arr[100]
        if (n > MAX_SIZE) n = MAX_SIZE;

        char *token = strtok(argv[3], ","); 
        int i = 0;
        // Thêm điều kiện i < MAX_SIZE
        while (token != NULL && i < n && i < MAX_SIZE) { 
            arr[i++] = atoi(token); 
            token = strtok(NULL, ","); 
        }
        // Cập nhật lại n thực tế (phòng trường hợp chuỗi nhập vào ít số hơn n khai báo)
        n = i; 
    } else {
        // Dữ liệu mẫu (Demo)
        int d[] = {2, 5, 8, 12, 16, 23, 38, 56, 72, 91}; 
        n = 10; 
        key = 23; 
        memcpy(arr, d, n*sizeof(int));
    }

    int low=0, high=n-1, mid=-1, found=0, step=0;
    
    printf("["); 
    print_state(step++, "Khởi tạo tìm kiếm.", arr, n, key, low, high, mid, found);
    
    while (low <= high) {
        mid = low + (high-low)/2; // Cách tính mid an toàn, tránh tràn số nguyên
        char msg[256]; 
        
        sprintf(msg, "Xét phần tử giữa (Mid) tại index %d, giá trị %d", mid, arr[mid]);
        print_state(step++, msg, arr, n, key, low, high, mid, found);
        
        if (arr[mid] == key) { 
            found=1; 
            print_state(step++, "Đã tìm thấy!", arr, n, key, low, high, mid, found); 
            break; 
        }
        
        if (arr[mid] < key) { 
            low = mid + 1; 
            sprintf(msg, "%d < %d (nhỏ hơn key) -> Bỏ nửa trái, tìm bên phải.", arr[mid], key); 
        } else { 
            high = mid - 1; 
            sprintf(msg, "%d > %d (lớn hơn key) -> Bỏ nửa phải, tìm bên trái.", arr[mid], key); 
        }
        print_state(step++, msg, arr, n, key, low, high, mid, found);
    }
    
    if(!found) print_state(step++, "Kết thúc: Không tìm thấy giá trị.", arr, n, key, -1, -1, -1, found);
    
    printf("]"); 
    return 0;
}