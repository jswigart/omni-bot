/*
 Copyright (c) 2011, Micael Hildenborg
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Micael Hildenborg nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY Micael Hildenborg ''AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL Micael Hildenborg BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "sha1.h"
#include <string.h>
#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#ifdef WIN32
double GetSystemTime(void)
{
    LARGE_INTEGER Frequency;
    BOOL UseHighPerformanceTimer=QueryPerformanceFrequency(&Frequency);
    // High performance counter available : use it
    LARGE_INTEGER CurrentTime;
    QueryPerformanceCounter(&CurrentTime);
    return static_cast<double>(CurrentTime.QuadPart) / Frequency.QuadPart;
}
#endif

int main(int argc, char* argv[])
{
    // Unittest
    unsigned char hash[20];
    char compare[41]; // 40 chars + a zero
    char hexstring[41]; // 40 chars + a zero
    char teststr[1001];
    int end = 0;
    compare[40] = hexstring[40] = 0;
    FILE* fh = fopen("1000sha1.txt", "r"); // file have been externally generated with PHP script.
    for (int i = 0; i < 1000; ++i)
    {
        sprintf(&teststr[end], "%d", i % 10);
        end = (int) strlen(teststr);
        fread(compare, 1, 40, fh);
        sha1::calc(teststr, end, hash);
        sha1::toHexString(hash, hexstring);
        if (strcmp(hexstring, compare) != 0)
        {
            printf("Unittest broken at: %d\n", i);
            return (1);
        }
    }
    fclose(fh);
    printf("Unittest OK.\n");

    // Start time check.
#ifdef WIN32
    double start = GetSystemTime();
#else
    timeval start;
    gettimeofday(&start, NULL);
#endif
    for (int i = 0; i < 1000000; ++i)
    {
        sha1::calc(teststr, (int) strlen(teststr), hash);
    }

#ifdef WIN32
    double stop=GetSystemTime();
    double time=stop - start;
#else
    timeval stop, result;
    gettimeofday(&stop, NULL);
    timersub(&stop,&start,&result);
    double time = result.tv_sec + result.tv_usec / 1000000.0;
#endif
    printf("Time: %f s\n", time);
    printf("Press enter...\n");
    scanf("Anything.");
    return (0);
}
