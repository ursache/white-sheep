#pragma once

extern "C"
void scan_kernel(
    const bool write,
    const int n,
    const float bias,
    const float * const __restrict__ height,
    float * const __restrict__ output,
    float * const __restrict__ states);
