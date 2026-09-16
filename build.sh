#!/bin/bash
# 影院玲珑构建包装：玲珑 prepare 阶段不允许项目根残留上次的输出文件
# （org.deepin.movie.install / install.log 由上次构建生成），先清理再构建。
# 打包机（已配置 gerrit SSH key）：./build.sh
# 无凭据机器（本地调试）：./build.sh --skip-fetch-source  （用 deps/deepin-gpuinfo 兜底源码）
cd "$(dirname "$0")"
rm -f org.deepin.movie.install install.log
exec ll-builder build "$@"
