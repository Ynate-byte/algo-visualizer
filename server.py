import subprocess
import os
import platform
from flask import Flask, render_template, jsonify, request

app = Flask(__name__, template_folder='templates')

def get_executable_path(name):
    """Xác định đường dẫn file thực thi (thêm .exe nếu là Windows)"""
    ext = ".exe" if platform.system() == "Windows" else ""
    return os.path.join("c_modules", name + ext)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/run/<algo>', methods=['POST'])
def run_algo(algo):
    """
    API chạy thuật toán. Nhận JSON -> Gọi Process C -> Trả về JSON Output.
    """
    exe_path = get_executable_path(algo)
    
    if not os.path.exists(exe_path):
        return jsonify({"error": f"Không tìm thấy file: {exe_path}. Hãy biên dịch lại code C."}), 500

    # Lấy dữ liệu từ Frontend
    data = request.json or {}
    args = [exe_path]

    # Xây dựng tham số dòng lệnh (argv) cho C
    if algo == 'binary':
        # binary [key] [size] [array_string]
        args.append(str(data.get('key', 0)))
        args.append(str(data.get('size', 0)))
        args.append(data.get('arr_str', ""))

    elif algo == 'huffman':
        # huffman [chars_str] [freqs_str]
        args.append(data.get('chars', ""))
        args.append(data.get('freqs', ""))

    elif algo == 'floyd':
        # floyd [size] [matrix_string]
        # QUAN TRỌNG: Truyền kích thước N trước, sau đó là chuỗi ma trận
        args.append(str(data.get('size', 4)))
        args.append(data.get('matrix', ""))

    try:
        # Gọi file C và bắt kết quả từ stdout
        result = subprocess.check_output(args, text=True, stderr=subprocess.PIPE)
        return result
    except subprocess.CalledProcessError as e:
        return jsonify({"error": "Lỗi Runtime C", "details": e.stderr}), 500
    except Exception as e:
        return jsonify({"error": "Lỗi Server Python", "details": str(e)}), 500

#if __name__ == '__main__':
#    app.run(debug=True, host='0.0.0.0', port=5000)

if __name__ == '__main__':
    port = int(os.environ.get("PORT", 5000))
    app.run(debug=False, host='0.0.0.0', port=port)