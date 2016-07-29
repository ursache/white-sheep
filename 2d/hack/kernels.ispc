export
void scan_kernel(
    uniform const bool write,
    uniform const int n,
    uniform const float bias,
    uniform const float height[],
    uniform float output[],
    uniform float states[])
{
    foreach( i = 0 ... n )
    {
	const float h = height[i];
	
	const bool p = h > bias + states[i];
	
	if (write)
	    output[i] = p;
	
	if (p)
	    states[i] = h - bias;
    }
}
