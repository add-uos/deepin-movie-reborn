#!/bin/bash
# 刷新 deps/deepin-gpuinfo 内嵌源码为 deepin-gpuinfo develop/snipe 分支最新，
# 刷新后需重新 git commit 提交到影院仓库。
# 用法: 在本脚本所在目录执行 ./refresh-gpuinfo.sh [deepin-gpuinfo本地克隆路径]
set -e
REPO="${1:-${GPUINFO_REPO:-/home/zhy/debug/deepin-gpuinfo}}"
BRANCH="${GPUINFO_BRANCH:-origin/develop/snipe}"
SRC="$(cd "$(dirname "$0")" && pwd)/deepin-gpuinfo"

cd "$REPO"
if ! git fetch origin develop/snipe 2>&1; then
    echo "WARN: git fetch 失败（无凭据?），使用本地 $BRANCH 缓存" >&2
fi
COMMIT=$(git rev-parse --short "$BRANCH")
echo "导出 $BRANCH ($COMMIT) -> $SRC"
rm -rf "$SRC"
mkdir -p "$SRC"
git archive "$BRANCH" | tar -x -C "$SRC"
echo "$COMMIT" > "$SRC/.source-commit"
echo "完成，记得 git add deps/deepin-gpuinfo 提交"
