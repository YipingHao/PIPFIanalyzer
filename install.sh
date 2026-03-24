#!/bin/bash

# ��ӡ������Ϣ
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

# Ĭ�ϲ���
OPTIMIZATION="-O2"
CLEAN=no
ONLY_CLEAN=no
DEBUG=no

# ���������в���
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

# ���ָ���� -C����ֻ clean ���˳�
if [[ "$ONLY_CLEAN" == "yes" ]]; then
  cd "$(dirname "$0")/tools/" || { echo "Failed to enter tools directory"; exit 1; }
  make clean
  echo "Cleaned."
  exit 0
fi

# ���ͬʱָ�� -g �� -o����ϲ�ʹ��
if [[ "$DEBUG" == "yes" && "$OPTIMIZATION" != "-O0" ]]; then
  MAKE_OPTION="$OPTIMIZATION -g"
elif [[ "$DEBUG" == "yes" ]]; then
  MAKE_OPTION="-g"
else
  MAKE_OPTION="$OPTIMIZATION"
fi

cd "$(dirname "$0")/tools/" || { echo "Failed to enter tools directory"; exit 1; }

# ִ�������������Ҫ��
if [[ "$CLEAN" == "yes" ]]; then
  make clean
fi

# ִ�б���
if [[ -n "$MAKE_OPTION" ]]; then
  make OPTION="$MAKE_OPTION"
  make test.exe OPTION="$MAKE_OPTION"
else
  make
  make test.exe
fi