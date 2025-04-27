import os
import subprocess
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import re
from tqdm import tqdm

# 定义输入文件和算法列表
input_files = [
    "dickens", "mozilla", "mr", "nci", "ooffice", "osdb",
    # "reymont", "samba", "sao", "webster", "x-ray", "xml"
]
algorithms = ["lz4", "lz4hc", "lzo", "lzo-rle", "zstd", "842"]
block_sizes = [1, 4]  # 分块大小 (以页数为单位)
iterations = [1, 5]  # 循环次数
page_shuffle_values = [0, 1]  # page_shuffle是否启用 (注意！0 或 1)

# 存储结果的列表
results = []

# 定义解析输出函数
def parse_output(output):
    # 正则表达式模式匹配
    result = {}

    # 匹配输出中的各个信息
    block_size_match = re.search(r"\[INFO\]: block size (\d+) pages", output)
    iterations_match = re.search(r"number of iterations (\d+)", output)
    file_size_match = re.search(r"\[INFO\]: file size (\d+)", output)
    nblocks_match = re.search(r"number of blocks (\d+)", output)
    compression_throughput_match = re.search(r"\[INFO\]: compression throughput ([\d\.]+) MiB/Second", output)
    compression_ratio_match = re.search(r"\[INFO\]: compression ratio \(original size / compressed size\) ([\d\.]+)", output)
    decompression_throughput_match = re.search(r"\[INFO\]: decompression throughput ([\d\.]+) MiB/Second", output)

    # 提取匹配结果并存储
    if block_size_match:
        result["block_size"] = int(block_size_match.group(1))
    if iterations_match:
        result["iterations"] = int(iterations_match.group(1))
    if file_size_match:
        result["file_size"] = int(file_size_match.group(1))
    if nblocks_match:
        result["nblocks"] = int(nblocks_match.group(1))
    if compression_throughput_match:
        result["compression_throughput"] = float(compression_throughput_match.group(1))
    if compression_ratio_match:
        result["compression_ratio"] = float(compression_ratio_match.group(1))
    if decompression_throughput_match:
        result["decompression_throughput"] = float(decompression_throughput_match.group(1))

    return result

# 定义函数运行命令并获取输出
def run_compression_test(input_file, algorithm, block_size, n_iteration, page_shuffle):
    cmd = [
        "./FastCompress",  # 压缩程序的路径
        os.path.join("data", input_file),  # 输入文件路径
        str(block_size),  # 分块大小
        str(n_iteration),  # 循环次数
        str(page_shuffle),  # 是否启用页面打乱
        algorithm  # 压缩算法
    ]
    
    try:
        # 调用外部压缩程序并捕获输出
        result = subprocess.run(cmd, capture_output=True, text=True, check=True)
        output = result.stdout
        
        # 打印输出以便调试
        # print(f"Output for {input_file} with {algorithm}, block_size={block_size}, iterations={n_iteration}, page_shuffle={page_shuffle}:\n")
        # print(output)  # 打印输出结果
        
        # 解析输出并打印解析结果
        outputlist = parse_output(output)
        # print(f"Parsed output: {outputlist}\n")  # 打印解析后的字典
        
        return outputlist
    except subprocess.CalledProcessError as e:
        print(f"Error running command: {cmd}")
        return None

# 计算总测试数量
total_tests = len(input_files) * len(algorithms) * len(block_sizes) * len(iterations) * len(page_shuffle_values)

# 初始化计数器
test_counter = 0

# 运行所有测试并记录结果
for input_file in input_files:
    for algorithm in algorithms:
        for block_size in block_sizes:
            for n_iteration in iterations:
                for page_shuffle in page_shuffle_values:
                    # 运行测试并获取输出
                    outputlist = run_compression_test(input_file, algorithm, block_size, n_iteration, page_shuffle)

                    # 更新计数器
                    test_counter += 1

                    # 打印并检查 outputlist 内容
                    if outputlist is not None:
                        results.append({
                            "input_file": input_file,
                            "algorithm": algorithm,
                            "page_shuffle": page_shuffle,
                            "block_size": outputlist.get("block_size", "N/A"),
                            "iterations": outputlist.get("iterations", "N/A"),
                            "file_size": outputlist.get("file_size", "N/A"),
                            "nblocks": outputlist.get("nblocks", "N/A"),
                            "compression_throughput": outputlist.get("compression_throughput", "N/A"),
                            "compression_ratio": outputlist.get("compression_ratio", "N/A"),
                            "decompression_throughput": outputlist.get("decompression_throughput", "N/A")
                        })
                    else:
                        print(f"Test failed for {input_file}, {algorithm}, block_size={block_size}, iterations={n_iteration}, page_shuffle={page_shuffle}")

                    # 显示进度条
                    print(f"Progress: {test_counter}/{total_tests} ({(test_counter / total_tests) * 100:.2f}%)", end='\r')

# 创建 DataFrame
df = pd.DataFrame(results)

# 输出到 CSV 文件
df.to_csv("compression_test_results.csv", index=False)

'''
# 生成图表
plt.figure(figsize=(10, 6))
pivot_df = df.pivot_table(values="throughput", index=["file", "algorithm"], columns="block_size", aggfunc=np.mean)

# 绘制热图
plt.title("Compression Throughput by Algorithm and Block Size")
plt.xlabel("Block Size (pages)")
plt.ylabel("File / Algorithm")
plt.imshow(pivot_df, cmap="viridis", aspect="auto", interpolation="nearest")
plt.colorbar(label="Throughput (MiB/s)")
plt.xticks(np.arange(len(pivot_df.columns)), pivot_df.columns)
plt.yticks(np.arange(len(pivot_df.index)), pivot_df.index)
plt.tight_layout()
plt.show()
'''
