// SIMD.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include<emmintrin.h>
#include<time.h>
#include<Windows.h>
#include <immintrin.h>
using namespace std;
const int N = 1024;
float elm[N][N] = {0};

void reset(float **test)
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            test[i][j] = elm[i][j];
        }
    }
    return;
}

//平凡算法
void simple(float **e)
{
    for (int k= 0; k < N; k++)
    {
        for (int j = k+1;j<N; j++)
            e[k][j] = e[k][j] / e[k][k];
           
        e[k][k] = 1.0;

        for (int i = k + 1; i < N; i++)
        {
            for (int j = k + 1; j < N; j++)
                e[i][j] = e[i][j] - e[i][k] * e[k][j];
            
            e[i][k] = 0;
        }
    }
    return;
}

//SSE算法
void sse_gauss(float** e)
{
    __m128 t1, t2, t3;
    for (int k = 0; k < N; k++)
    {
        float temp[4] = { e[k][k],e[k][k],e[k][k],e[k][k] };
        t2 = _mm_loadu_ps(temp);
        int j = k + 1;
        for (j; j< N-4; j += 4)
        {
            t1 = _mm_loadu_ps(e[k] + j);
            t3 = _mm_div_ps(t1, t2);
            _mm_store_ps(e[k]+j, t3);
        }
        for (j; j < N; j++)
            e[k][j] = e[k][j] / e[k][k];
        
        e[k][k] = 1.0;

     
        for (int i = k + 1; i < N; i++)
        {
            float temp[4] = { e[i][k],e[i][k],e[i][k],e[i][k] };
            t1 = _mm_loadu_ps(temp);
            int j = k + 1;
            for (j; j < N-4; j+=4)
            {
                t2 = _mm_loadu_ps(e[k] + j);
                t3 = _mm_loadu_ps(e[i] + j);
                t2 = _mm_mul_ps(t2, t1);
                t3 = _mm_sub_ps(t3, t2);
                _mm_store_ps(e[i] + j, t3);
            }
            for (j; j < N; j++)
            {
                e[i][j] = e[i][j] - e[i][k] * e[k][j];
            }
            e[i][k] = 0;
        }
   }
    return;
}

//AVX算法
void avx_gauss(float** e)
{
    __m256 t1, t2, t3;
    for (int k = 0; k < N; k++)
    {
        float temp1[8] = { e[k][k], e[k][k], e[k][k], e[k][k], e[k][k], e[k][k], e[k][k], e[k][k] };
        t1 = _mm256_loadu_ps(temp1);
        int j = k + 1;
        for (j; j < N - 7; j += 8)
        {
            t2 = _mm256_loadu_ps(e[k] + j);
            t3 = _mm256_div_ps(t2, t1);
            _mm256_storeu_ps(e[k] + j, t3);
        }
        for (j; j < N; j++)
            e[k][j] = e[k][j] / e[k][k];

        e[k][k] = 1.0;

        for (int i = k + 1; i < N; i++)
        {
            float temp2[8] = { e[i][k], e[i][k], e[i][k], e[i][k], e[i][k], e[i][k], e[i][k], e[i][k] };
            t1 = _mm256_loadu_ps(temp2);
            j = k + 1;
            for (j; j < N - 7; j += 8)
            {
                t2 = _mm256_loadu_ps(e[i] + j);
                t3 = _mm256_loadu_ps(e[k] + j);
                t3 = _mm256_mul_ps(t1, t3);
                t2 = _mm256_sub_ps(t2, t3);
                _mm256_storeu_ps(e[i] + j, t2);
            }
            for (j; j < N; j++)
                e[i][j] = e[i][j] - e[i][k] * e[k][j];

            e[i][k] = 0;
        }
    }
}

int main()
{
  
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            elm[i][j] = (rand() % 100);
        }
    }
    float** test = new float*[N];
    for (int i = 0; i < N; i++)
    {
        test[i] = new float[N];
    }

    reset(test);
   
    srand(time(NULL));
    LARGE_INTEGER timeStart;	//开始时间
    LARGE_INTEGER timeEnd;		//结束时间

    LARGE_INTEGER frequency;	//计时器频率
    QueryPerformanceFrequency(&frequency);
    double quadpart = (double)frequency.QuadPart;//计时器频率

    //平凡算法
    QueryPerformanceCounter(&timeStart);
    simple(test);
    QueryPerformanceCounter(&timeEnd);
    double _Simple = (timeEnd.QuadPart - timeStart.QuadPart) / quadpart;
    printf("Simple:%f\n", _Simple);
    cout << endl;
    reset(test);
  
    //SSE算法
    QueryPerformanceCounter(&timeStart);
    sse_gauss(test);
    QueryPerformanceCounter(&timeEnd);
    double _SSE_Gauss = (timeEnd.QuadPart - timeStart.QuadPart) / quadpart;
    printf("SSE_Gauss:%f\n", _SSE_Gauss);
    cout << endl;
    reset(test);

    //AVX算法
    QueryPerformanceCounter(&timeStart);
    avx_gauss(test);
    QueryPerformanceCounter(&timeEnd); 
    double _AVX_Gauss = (timeEnd.QuadPart - timeStart.QuadPart) / quadpart;
    printf("AVX_Gauss:%f\n", _AVX_Gauss);
    cout << endl;
    reset(test);

    system("pause");
    return 0;
}


