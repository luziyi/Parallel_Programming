#!/bin/bash
> run.log
power=15
runs=2
module unload openmpi/mpi-x-gcc9.3.0
module load openmpi/4.1.4-mpi-x-gcc9.3.0
mpic++ -o lab3.o lab3.c

for size in {1..64}; do
    echo "Running with core=$size" >> run.log
    total_time=0
    # 多次运行以获取平均值
    for ((i=1; i<=runs; i++)); do
        # 使用time命令并通过grep提取real时间
        run_time=$( { time yhrun -p thcp1 -N 4 -n $size ./lab3.o $power ; } 2>&1 | grep "real" | awk '{print $2}')
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