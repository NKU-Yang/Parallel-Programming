// SIMD.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <xmmintrin.h>
#include <iostream>
#include<emmintrin.h>
using namespace std;

int sum3_Intrinsics(int* a, int size)
{
    if (NULL == a)    return 0;
    if (size < 0)    return 0;

    int s = 0;    // 返回值
    __m128i xidSum = _mm_setzero_si128();    // 累积。[SSE2] 赋初值0
    __m128i xidLoad;    // 加载
    int cntBlock = size / 4;    // 块数。SSE寄存器能一次处理4个DWORD
    int cntRem = size & 3;    // 剩余数量
    __m128i* p = (__m128i*)a;
    for (int i = 0; i < cntBlock; ++i)
    {
        xidLoad = _mm_load_si128(p);    // [SSE2] 加载
        xidSum = _mm_add_epi32(xidSum, xidLoad);    // [SSE2] 带符号32位紧缩加法
        ++p;
    }

    // 处理剩下的
    int* q = (int*)p;
    for (int i = 0; i < cntRem; ++i)    s += q[i];

    // 将累加值合并
    xidSum = _mm_add_epi32(xidSum, xidSum);    // [SSSE3] 带符号32位水平加法
    xidSum = _mm_add_epi32(xidSum, xidSum);
    s += _mm_cvtsi128_si32(xidSum);    // [SSE2] 返回低32位

    return s;
}
int main()
{
    
}


