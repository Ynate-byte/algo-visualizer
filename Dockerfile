# Sử dụng Python làm môi trường nền (dựa trên Linux Debian)
FROM python:3.9-slim

# 1. Cài đặt GCC (Trình biên dịch C)
RUN apt-get update && apt-get install -y gcc libc-dev

# 2. Tạo thư mục làm việc
WORKDIR /app

# 3. Cài thư viện Flask
RUN pip install flask

# 4. Copy toàn bộ code từ máy tính vào trong Docker
COPY . .

# 5. Biên dịch Code C ngay trong quá trình Build Docker
# (Lưu ý: Trên Linux không cần đuôi .exe)
WORKDIR /app/c_modules
RUN gcc binary.c -o binary
RUN gcc huffman.c -o huffman
RUN gcc floyd.c -o floyd

# Quay lại thư mục gốc để chuẩn bị chạy Python
WORKDIR /app

# 6. Mở cổng 5000
EXPOSE 5000

# 7. Lệnh chạy server khi container khởi động
CMD ["python", "-u", "server.py"]