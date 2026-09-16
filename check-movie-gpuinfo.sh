#!/bin/bash
# 验证玲珑影院包内的 libgpuinfo：来源、ldd、dlopen 加载
# 用法: ll-builder run bash /project/check-movie-gpuinfo.sh
LIB=/opt/apps/org.deepin.movie/files/lib/${TRIPLET:-x86_64-linux-gnu}

echo "=== 1. 包内 libgpuinfo 文件:"
ls -la $LIB/libgpuinfo* 2>/dev/null

echo
echo "=== 2. ldd 依赖解析:"
ldd $LIB/libgpuinfo.so.0.1.0 | grep -E "not found" && echo ">>> 有缺失!" || echo ">>> 所有依赖可解析"

echo
echo "=== 3. ldconfig 按名查找:"
ldconfig -p | grep -i gpuinfo

echo
echo "=== 4. 模拟影院 QLibrary::resolve('gpuinfo_get_vo') 的 dlopen 测试:"
cat > /tmp/dltest.c <<'EOF'
#include <dlfcn.h>
#include <stdio.h>
int main() {
    void *h = dlopen("libgpuinfo.so", RTLD_NOW);
    if (!h) { printf("dlopen FAILED: %s\n", dlerror()); return 1; }
    printf("dlopen OK: libgpuinfo.so\n");
    const char* (*fn)(void) = dlsym(h, "gpuinfo_get_vo");
    if (!fn) { printf("dlsym gpuinfo_get_vo FAILED: %s\n", dlerror()); return 1; }
    printf("dlsym gpuinfo_get_vo OK, return: %s\n", fn() ? fn() : "(NULL)");
    dlclose(h);
    return 0;
}
EOF
gcc /tmp/dltest.c -o /tmp/dltest -ldl && /tmp/dltest

echo
echo "=== 5. csv 配置:"
ls -la /opt/apps/org.deepin.movie/files/share/deepin-gpuinfo/gpuinfolib.csv
