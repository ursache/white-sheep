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
    for(int j = 0; j < nstates; ++j)
    {
	const int x = j + bias;
	
	if (x >= 0 && x < noutput)
	{
	    const float h = height[x];
	    
	    const bool p = h > alength + states[j];
	    
	    if (write)
		output[x] = p;
	
	    if (p)
		states[j] = h - alength;
	}
    }
}
    
