#!/bin/bash
# 功能：带参数提交所有已跟踪文件的 Git 修改
# 用法：./git_commit.sh "提交信息"

# 验证参数是否存在
if [ $# -eq 0 ]; then
    echo -e "\033[31m错误：请提供提交信息参数\033[0m"
    echo "示例：./git_commit.sh \"修复用户登录逻辑\""
    exit 1
fi

# 执行带参数的 Git 提交
git commit -a -m "$1"
git push server
git push

# 检查执行结果
if [ $? -eq 0 ]; then
    echo -e "\033[32m提交成功！提交信息：$1\033[0m"
else
    echo -e "\033[31m提交失败，请检查 Git 状态\033[0m"
fi
