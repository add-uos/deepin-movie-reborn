#!/bin/bash
# 影院玲珑构建包装：玲珑 prepare 阶段不允许项目根残留上次的输出文件
# （org.deepin.movie.install / install.log 由上次构建生成），先清理再构建。
cd "$(dirname "$0")"
rm -f org.deepin.movie.install install.log
exec ll-builder build "$@"
