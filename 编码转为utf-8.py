# -*- coding: utf-8 -*-
import os
import codecs
from chardet import detect

# ====================== 你只需要修改这里 ======================
# 目标文件夹路径（Windows 用 / 或者 \\ 都可以）
TARGET_DIR = r"./"
# ============================================================

# 需要转换的文件后缀
FILE_EXTENSIONS = (".c", ".h")

def convert_to_utf8(file_path):
    """
    将单个文件转换为 UTF-8 编码
    """
    try:
        # 读取文件二进制内容
        with open(file_path, "rb") as f:
            raw_data = f.read()

        # 自动检测原编码
        encoding = detect(raw_data)["encoding"]
        if not encoding:
            print(f"⚠️  无法识别编码: {file_path}")
            return

        encoding = encoding.lower()
        # 常见中文编码别名处理
        if encoding in ["gb2312", "gbk", "gb18030"]:
            encoding = "gbk"
        elif encoding == "utf-8-sig":
            encoding = "utf-8"

        # 如果已经是 UTF-8，跳过
        if encoding == "utf-8":
            print(f"✅ 已是 UTF-8: {file_path}")
            return

        # 解码并重新编码为 UTF-8
        content = raw_data.decode(encoding, errors="replace")
        with codecs.open(file_path, "w", "utf-8") as f:
            f.write(content)

        print(f"✅ 转换成功: {file_path}  | 原编码: {encoding} -> UTF-8")

    except Exception as e:
        print(f"❌ 处理失败: {file_path}  | 错误: {str(e)}")

def batch_convert(dir_path):
    """
    递归遍历目录，批量转换文件
    """
    if not os.path.isdir(dir_path):
        print(f"❌ 目录不存在: {dir_path}")
        return

    print(f"🚀 开始扫描目录: {dir_path}\n")

    for root, dirs, files in os.walk(dir_path):
        for file in files:
            if file.lower().endswith(FILE_EXTENSIONS):
                file_path = os.path.join(root, file)
                convert_to_utf8(file_path)

    print("\n🎉 全部处理完成！")

if __name__ == "__main__":
    batch_convert(TARGET_DIR)