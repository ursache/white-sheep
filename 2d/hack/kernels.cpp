extern "C"
void scan(
    const int bias,
    const float * const height,
    const float alength,
    const bool write,
    float * const states,
    const int nstates,
    float * const output,
    const int noutput)
{
    const int p0 = -bias;
    const int p1 = noutput - bias;

    const int s = p0 < p1 ? p0 : p1;
    const int e = p0 + p1 - s;
    const int n = e - s;
    
    for(int i = 0; i < n; ++i)
    {
	const int j = s + i;
	const int x = s + bias + i;
	
	const float h = height[x];
	    
	const bool p = h > alength + states[j];
	
	if (write)
	    output[x] = p;
	
	if (p)
	    states[j] = h - alength;
    }
}
    
