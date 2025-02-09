#!/bin/bash

# 输出文件
output_file="results.csv"

# 清空结果文件
echo "schedule,chunk_size,time" > $output_file

# 调度策略数组
schedules=("static" "dynamic" "guided")

# Chunk size 数组
chunk_sizes=(1 2 4 8 16)

# 运行实验
for schedule in "${schedules[@]}"; do
    for chunk in "${chunk_sizes[@]}"; do
        export OMP_SCHEDULE="$schedule,$chunk"
        echo "Running: schedule=$schedule, chunk_size=$chunk"
        # 运行程序并获取时间
        start_time=$(date +%s%N) # 开始时间（纳秒）
        ./lab3_schedule 1000 4 > /dev/null 2>&1 # 静默运行程序
        end_time=$(date +%s%N) # 结束时间（纳秒）
        elapsed_time=$(( (end_time - start_time) / 1000000 )) # 计算运行时间（毫秒）
        echo "$schedule,$chunk,$elapsed_time" >> $output_file
    done
done

echo "Experiment completed! Results saved to $output_file."
