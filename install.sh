#!/bin/bash

# 打印帮助信息
usage() {
  echo "Usage: $0 [-g] [-o <0|1|2|3>] [-c] [-C] [-h]"
  echo ""
  echo "Options:"
  echo "  -g           Enable debug info (-g)"
  echo "  -o <level>   Set optimization level (0, 1, 2 or 3)"
  echo "  -c           Clean build files before building"
  echo "  -C           Only clean and exit"
  echo "  -h           Show this help message"
  exit 0
}

# 默认参数
OPTIMIZATION="-O0"
CLEAN=no
ONLY_CLEAN=no
DEBUG=no

# 解析命令行参数
while getopts "gco:Ch" opt; do
  case $opt in
    g)
      DEBUG=yes
      ;;
    o)
      if [[ "$OPTARG" =~ ^[0123]$ ]]; then
        OPTIMIZATION="-O$OPTARG"
      else
        echo "Error: -o argument must be 0, 1, 2 or 3." >&2
        exit 1
      fi
      ;;
    c)
      CLEAN=yes
      ;;
    C)
      ONLY_CLEAN=yes
      ;;
    h)
      usage
      ;;
    *)
      echo "Invalid option." >&2
      usage
      ;;
  esac
done

# 如果指定了 -C，则只 clean 并退出
if [[ "$ONLY_CLEAN" == "yes" ]]; then
  cd "$(dirname "$0")/tools/" || { echo "Failed to enter tools directory"; exit 1; }
  make clean
  echo "Cleaned."
  exit 0
fi

# 如果同时指定 -g 和 -o，则合并使用
if [[ "$DEBUG" == "yes" && "$OPTIMIZATION" != "-O0" ]]; then
  MAKE_OPTION="$OPTIMIZATION -g"
elif [[ "$DEBUG" == "yes" ]]; then
  MAKE_OPTION="-g"
else
  MAKE_OPTION="$OPTIMIZATION"
fi

cd "$(dirname "$0")/tools/" || { echo "Failed to enter tools directory"; exit 1; }

# 执行清理（如果需要）
if [[ "$CLEAN" == "yes" ]]; then
  make clean
fi

# 执行编译
if [[ -n "$MAKE_OPTION" ]]; then
  make OPTION="$MAKE_OPTION"
  make test.exe OPTION="$MAKE_OPTION"
else
  make
  make test.exe
fi