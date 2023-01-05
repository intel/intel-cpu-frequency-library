/*
Copyright (c) 2022, Intel Corporation All rights reserved. 

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following
  disclaimer. 
* Neither the name of Intel Corporation nor the names of its contributors may be used to endorse or promote products
  derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

typedef long long t_cpuclock;
typedef long long t_usec;

#ifdef __GNUC__

#include <sched.h>
#include <unistd.h>
#include <sys/time.h>
#include <immintrin.h>

static __inline
t_usec get_usec()
{
    struct timeval t_usecime;
    gettimeofday(&t_usecime, NULL);
    return ((t_usec)(t_usecime.tv_sec) * 1000000 + t_usecime.tv_usec);
}


static __inline 
t_cpuclock get_tsc(unsigned int* ppid)
{
    unsigned int lo, hi, pid;
    __asm__ __volatile__("rdtscp"
        : "=a" (lo), "=d" (hi), "=c" (pid)
        : : "%ebx");
    if (ppid) *ppid = pid;

    return ((t_cpuclock)hi << 32) | lo;
}

static  __attribute__((noinline)) 
void fixed_loop(int count)
{
    // executes the loop in 'count' + fixed_overhead number of cycles
    // due to 1-cycle latency dependency or all non-ancient CPUs
    __asm__ __volatile__(
        "          lfence"
        "\n        .p2align 3"
        "\n    2:"
        "\n        and %[cnt], %[v1]"
        "\n        sub %[cnt], %[v2]"
        "\n        jg  2b"
        : : [cnt] "r" (count), [v1] "r" (-1), [v2] "r" (2) : "cc");
}


#elif _WIN32
#include <windows.h>

static __forceinline
t_usec get_usec()
{
    static t_cpuclock  perf_freq;
    static BOOL init_freq = 0;

    __int64  counter;
    if (!init_freq)
        init_freq = QueryPerformanceFrequency((LARGE_INTEGER*)&perf_freq);

    QueryPerformanceCounter((LARGE_INTEGER*)&counter);

    return counter * 1000000 / perf_freq; // convert to usec
}

static __forceinline
t_cpuclock get_tsc(unsigned int* ppid)
{
    unsigned int tsc_aux_;
    return (t_cpuclock)__rdtscp(ppid ? ppid : &tsc_aux_);
}

/* implemented in a separate MASM file since VS doesn't support inline assembly */
void __fastcall fixed_loop(int);

#else
#error "Unknown platform"
#endif

static float get_curr_to_base_freq_ratio(int attempts)
{
    const int fixed_loop_clocks = 10000;
    
    for (int i = 0; i < attempts; i++)
    {
        unsigned int pid1, pid2, pid3;

        t_cpuclock rdtsc_1 = get_tsc(&pid1);
        fixed_loop(fixed_loop_clocks); // single charge
        t_cpuclock rdtsc_2 = get_tsc(&pid2);
        fixed_loop(2 * fixed_loop_clocks); // double charge
        t_cpuclock rdtsc_3 = get_tsc(&pid3);

        t_cpuclock clocks1 = rdtsc_2 - rdtsc_1;
        t_cpuclock clocks2 = rdtsc_3 - rdtsc_2;
        t_cpuclock clocks = clocks2 - clocks1; // single charge minus common overhead

        // clocks should be close to half of clocks2 (clocks > clocks2/2.125)
        if (pid1 == pid2 && pid2 == pid3 && clocks < clocks1 && (clocks << 1) + (clocks >> 3) > clocks2)
        {
            return ((float)fixed_loop_clocks) / clocks;
        }
    }

    return 0.0f;
}

static float measure_base_freq_hz_once(int attempts)
{
    const int   test_clocks = 500000;
    const int   test_margin = (test_clocks >> 7);
    t_usec      usec = 0, usec1, usec2;
    t_cpuclock  clks = 0, clks1, clks2;
    int         i;
    unsigned int pid;

    for (i = 0; i < attempts; ++i)
    {
        usec1 = get_usec();
        clks1 = get_tsc(&pid);
		fixed_loop(test_clocks);
        clks1 = get_tsc(&pid) - clks1;
        usec1 = get_usec() - usec1;

        usec = get_usec();
        clks = get_tsc(&pid);
        fixed_loop(test_clocks << 1);
        clks = get_tsc(&pid) - clks;
        usec = get_usec() - usec;

        usec2 = get_usec();
        clks2 = get_tsc(&pid);
        fixed_loop(test_clocks);
        clks2 = get_tsc(&pid) - clks2;
        usec2 = get_usec() - usec2;

        clks -= clks1; /* removing const overhead */
        usec -= usec1;

        /* checks to skip results affected by interrupts/state change/etc */
        if (!(clks < 0 || clks1 < 0 || clks > clks1
            || clks1 >(clks + test_margin)
            || clks2 > (clks1 + test_margin) || clks1 > (clks2 + test_margin)
            || usec < 0 || usec1 < 0 || usec > usec1
            || usec1 >(usec + 1) || usec > (usec1 + 1)))
        {
            return (1e6f * clks) / usec;
        }
    }

    return 0.0f; /* error value */
}

static float measure_base_freq_hz(int attempts)
{
    measure_base_freq_hz_once(1);

    return (measure_base_freq_hz_once(attempts) +
            measure_base_freq_hz_once(attempts) +
            measure_base_freq_hz_once(attempts) +
            measure_base_freq_hz_once(attempts)) / 4;
}

static float round_freq(float freq, float rndtohz)
{
    return rndtohz * (int)((freq + rndtohz / 2) / rndtohz);
}

float get_base_cpu_freq_hz(void)
{
    static float s_base_freq_hz = -1.f;
    if (s_base_freq_hz == -1.f)
        s_base_freq_hz = round_freq(measure_base_freq_hz(1000), 100e6f /* rounded to 100 MHz */);

    return s_base_freq_hz;
}

float get_curr_cpu_freq_hz(void)
{
    float ratio = get_curr_to_base_freq_ratio(10);
    return round_freq(get_base_cpu_freq_hz() * ratio, 100e6f /* rounded to 100 MHz */);
}