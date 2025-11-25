#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_json_array(int arr[], int n) {
    printf("["); for(int i=0; i<n; i++) { printf("%d", arr[i]); if(i<n-1) printf(", "); } printf("]");
}
void print_state(int step, char* msg, int arr[], int n, int key, int low, int high, int mid, int found) {
    if (step > 0) printf(",\n");
    printf("{\"step\": %d, \"msg\": \"%s\", \"key\": %d, \"low\": %d, \"high\": %d, \"mid\": %d, \"found\": %d", step, msg, key, low, high, mid, found);
    if (step == 0) { printf(", \"data\": "); print_json_array(arr, n); }
    printf("}");
}
int main(int argc, char *argv[]) {
    int arr[100], n=0, key=0;
    if (argc == 4) {
        key = atoi(argv[1]); n = atoi(argv[2]);
        char *token = strtok(argv[3], ","); int i = 0;
        while (token != NULL && i < n) { arr[i++] = atoi(token); token = strtok(NULL, ","); }
    } else {
        int d[] = {2, 5, 8, 12, 16, 23, 38, 56, 72, 91}; n=10; key=23; memcpy(arr, d, n*sizeof(int));
    }
    int low=0, high=n-1, mid=-1, found=0, step=0;
    printf("["); print_state(step++, "Khởi tạo.", arr, n, key, low, high, mid, found);
    while (low <= high) {
        mid = low + (high-low)/2;
        char msg[200]; sprintf(msg, "Mid tại %d giá trị %d", mid, arr[mid]);
        print_state(step++, msg, arr, n, key, low, high, mid, found);
        if (arr[mid] == key) { found=1; print_state(step++, "Đã tìm thấy!", arr, n, key, low, high, mid, found); break; }
        if (arr[mid] < key) { low=mid+1; sprintf(msg, "%d < %d -> Tìm bên phải", arr[mid], key); }
        else { high=mid-1; sprintf(msg, "%d > %d -> Tìm bên trái", arr[mid], key); }
        print_state(step++, msg, arr, n, key, low, high, mid, found);
    }
    if(!found) print_state(step++, "Không tìm thấy.", arr, n, key, -1, -1, -1, found);
    printf("]"); return 0;
}