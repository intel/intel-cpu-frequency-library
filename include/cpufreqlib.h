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

#ifndef CPUFREQ_H_INCLUDED
#define CPUFREQ_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/* Get nominal CPU frequency in hertzs - stable since OS boot.
   It also corresponds to the frequency of CPU's time stamp counter. 
   Returns 0 if it fails to detect base frequency.
*/
float get_base_cpu_freq_hz(void);

/* Get current CPU frequency in hertzs - dynamic over time. Can be higher or lower than the base frequency.
   Returns 0 if it fails to detect current frequency.
*/
float get_curr_cpu_freq_hz(void);

#ifdef __cplusplus
}
#endif

#endif // CPUFREQ_H_INCLUDED