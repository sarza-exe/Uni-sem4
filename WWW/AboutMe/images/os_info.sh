#!/bin/bash
# View OS info every 1 second

convert_speed() {
    local bytes=$1
    if (( bytes < 1024 )); then
        echo "${bytes} B/s"
    elif (( bytes < 1048576 )); then
        echo "$(echo "scale=2; $bytes/1024" | bc) KB/s"
    else
        echo "$(echo "scale=2; $bytes/1048576" | bc) MB/s"
    fi
}

get_network_base()
{
    local received=($(awk '{print $2}' /proc/net/dev))
    local sent=($(awk '{print $10}' /proc/net/dev))

    for ((i=2; i<${#received[@]}; i++)); do
        old_sum_usage=$((old_sum_usage + received[i]))
    done

    for ((i=1; i<${#sent[@]}; i++)); do
        old_sum_usage=$((old_sum_usage + sent[i]))
    done   
}

get_network_usage()
{
    local received=($(awk '{print $2}' /proc/net/dev))
    local sent=($(awk '{print $10}' /proc/net/dev))

    sum_usage=0

    for ((i=2; i<${#received[@]}; i++)); do
        sum_usage=$((sum_usage + received[i]))
    done

    for ((i=1; i<${#sent[@]}; i++)); do
        sum_usage=$((sum_usage + sent[i]))
    done

    #bytes transmitted in last sec
    usage=$((sum_usage - old_sum_usage))
    old_sum_usage=$sum_usage

    #change max usage if the new one is greater
    if [[ $max_usage -lt $usage ]]; then
        max_usage=$usage
    fi

    #check if the last value on the chart was maximum
    #then we have to find new max
    find_new_max=0;
    if [[ $max_usage -lt ${past_usage[14]} ]]; then
        find_new_max=1
    fi

    #move all elements one to right and insert new value on place 0
    for ((i=14; i>0; i--)); do
        past_usage[i]=${past_usage[i-1]}
    done
    past_usage[0]=$usage

    # find new maximum if we deleted old in last step
    if [[ $find_new_max -gt 0 ]]; then
        max_usage=1
        for ((i=0; i<15; i++)); do
            if [[ $max_usage -lt ${past_usage[i]} ]]; then
            max_usage=${past_usage[i]}
            fi
        done
    fi

    y=()
    #scale internet usage to values from 0 to 10
    for ((j=0; j<15; j++)); do
        y+=($(((past_usage[j] * 10) / max_usage)))
    done

    #draw usage
    for ((i=10; i>0; i--)); do
    echo -ne "\e[0;41m \e[0;0m"
        for ((j=0; j<15; j++)); do
            echo -n " "
            if [[ ${y[j]} -ge $i ]]; then
                echo -ne "\e[0;46m \e[0;0m"
            else
                echo -n " "
            fi
        done
        echo ""
    done
    
    for ((i=0; i<=30; i++)); do
        echo -ne "\e[0;41m \e[0;0m"
    done

    echo ""
    echo -e "Inernet Speed:\t$(convert_speed $usage)"
}

get_cpu_usage()
{
    local count=2
    local line=$(awk -v cnt="$count" 'NR==cnt' /proc/stat)
    local line_split=($line)
    local cores_info=()

    while [[ $line_split[0] == cpu* ]]; do
        cores_info+=("$line")
        count=$((count + 1))
        line=$(awk -v cnt="$count" 'NR==cnt' /proc/stat)
        line_split=($line)
    done

    #check if length of cores_info_old is greater (gt) then 0
    if [[ ${#cores_info_old[@]} -gt 0 ]]; then

        local index=0

        echo "Usage of cores:"
        
        for core in "${cores_info[@]}"; do
            local core_data=($core)
            local core_data_old=(${cores_info_old[index]})

            local sum_old=0
            local sum=0

            # Loop starting from index 1
            for ((i=1; i<${#core_data[@]}; i++)); do
                sum_old=$((sum_old + core_data_old[i]))
                sum=$((sum + core_data[i]))
            done

            sum_old=$((sum_old - core_data_old[5]))
            sum=$((sum - core_data[5]))
            local total_old=$sum_old
            local total=$sum

            sum_old=$((sum_old - core_data_old[4]))
            sum=$((sum - core_data[4]))
            local delta=$((sum - sum_old))
            local total_delta=$((total - total_old))

            local usage=$(echo "scale=5; $delta / $total_delta * 100" | bc)

            echo -e "${core_data[0]}:\t$usage%"

            index=$((index + 1))
        done
    
    fi

    cores_info_old=("${cores_info[@]}")

}

get_power_info()
{
    #microamperes
    total=$(awk -F'=' 'NR==10 {print $2}' /sys/class/power_supply/BAT0/uevent)
    current=$(awk -F'=' 'NR==11 {print $2}' /sys/class/power_supply/BAT0/uevent)
    power=$((current * 100 / total))

    echo -e "Battery Status:\t$power%"
}

get_loadavg_info()
{
    #number of the jobs in the run queue (R) averaged over 1 minute
    loadavg_1min=$(awk '{print $1}' /proc/loadavg)
    echo -e "Average Jobs in Run Queue per Minute:\t$loadavg_1min"
}

get_mem_info()
{
    #mem_free is memory that has zeros written
    #mem_available An estimate of how much memory is available for  starting
    #new applications, without swapping.
    mem_total=$(awk '/^MemTotal:/ {print $2}' /proc/meminfo)
    mem_available=$(awk '/^MemAvailable:/ {print $2}' /proc/meminfo)
    mem_usage=$(((mem_total-mem_available) * 100 /mem_total))

    echo -e "RAM usage:\t$mem_usage%"
}

old_sum_usage=0

past_usage=()
for ((i=0; i<15; i++)); do
        past_usage+=(0)
    done
max_usage=1

cores_info_old=()

main()
{
    
    get_cpu_usage
    get_network_base

    while true; do
        sleep 1
        clear
        get_network_usage
        get_cpu_usage
        get_power_info
        get_loadavg_info
        get_mem_info
    done

}

main