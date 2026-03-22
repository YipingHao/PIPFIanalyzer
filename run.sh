#!/bin/bash



program="./test/test.exe"


# 默认值
debug_mode=false
gdb_args=()
input_file="parameter/para.txt"
output_file="output/"

# 显示用法帮助
usage() {
    echo "Usage: $0 [-g [gdb_options...]] [-i input_file] [-o output_file]"
    echo "  -g     : Use gdb to launch a.exe, followed by optional gdb arguments."
    echo "  -i arg : Specify input file (default: input.txt)"
    echo "  -o arg : Specify output file (default: output.txt)"
    echo "  -h           Show this help message"
    
}

# 手动解析参数
while [[ $# -gt 0 ]]; do
    case "$1" in
        -g)
            debug_mode=true
            shift  # 跳过 "-g"

            # 收集所有非选项参数作为 gdb 参数，直到遇到以 "-" 开头的选项
            while [[ $# -gt 0 && ! "$1" == -* ]]; do
                gdb_args+=("$1")
                shift
            done
            ;;
        -i)
            if [[ $# -lt 2 ]]; then
                echo "-i requires an argument." >&2
                usage
                exit 1
            fi
            input_file="$2"
            shift 2
            ;;
        -I)
            if [[ $# -lt 2 ]]; then
                echo "-i requires an argument." >&2
                usage
                exit 1
            fi
            input_file="./parameter/"
            input_file+="$2"
            shift 2
            ;;
        -o)
            if [[ $# -lt 2 ]]; then
                echo "-o requires an argument." >&2
                usage
                exit 1
            fi
            output_file="$2"
            shift 2
            ;;
        -h)
            usage
            exit 0
            ;;
        *)
            echo "Unknown option: $1" >&2
            usage
            exit 1
            ;;
    esac
done

if [ -d "$output_file" ]; then
    :
else
    # Try to create the directory
    mkdir "$output_file"
    # Check if directory creation failed
    if [ $? -ne 0 ]; then
        echo "Error: Failed to create directory '$output_file'. Check permissions or path validity."
        exit 1
    fi
fi


# 构造并执行命令
if $debug_mode; then
	gdb "${gdb_args[@]}" --args "$program" "$input_file" "$output_file"
else
	"$program" "$input_file" "$output_file"
fi


