#!/bin/bash
> run.log
# 定义要测试的核心数数组
cores=(1 2 4 8 16 32 64)
# 定义运行的次数
runs=5
n=150
# 对每个核心数进行循环
for core in "${cores[@]}"; do
    echo "Running with core=$core" >> run.log
    total_time=0
    # 多次运行以获取平均值
    for ((i=1; i<=runs; i++)); do
        # 使用time命令并通过grep提取real时间
        run_time=$( { time yhrun -p thcp1 ./lab2.o $n $core; } 2>&1 | grep "real" | awk '{print $2}')
        # 将real时间转换为秒
        # 假设run_time的格式为minutes:seconds，比如0m1.234s
        min=$(echo $run_time | cut -d'm' -f1)
        sec=$(echo $run_time | cut -d'm' -f2 | sed 's/s//')
        # 计算总秒数
        total_sec=$(echo "$min * 60 + $sec" | bc)
        total_time=$(echo "$total_time + $total_sec" | bc)
        # 输出每次运行的时间到日志
        echo "Run $i: $total_sec sec" >> run.log
    done
    # 计算平均运行时间
    avg_time=$(echo "scale=3; $total_time / $runs" | bc)
    echo "Average time for core $core: $avg_time sec" >> run.log
    echo "" >> run.log
done

echo "All done."