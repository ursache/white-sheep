extern "C"
void scan_kernel(
    const bool write,
    const int n,
    const float bias,
    const float * const __restrict__ height,
    float * const __restrict__ output,
    float * const __restrict__ states)
{
    for(int i = 0; i < n; ++i)
    {
	const float h = height[i];
	
	const bool p = h > bias + states[i];
	
	if (write)
	    output[i] = p;
	
	if (p)
	    states[i] = h - bias;
    }
}
