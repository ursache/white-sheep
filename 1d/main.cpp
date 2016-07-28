#include <cstdio>
#include <cmath>
#include <cstdlib>

int main()
{
    const int N = 100; //number of texels

    float h[N];//height field

    for(int i = 0; i < N; ++i)
	h[i] = (sin(0.1 * i + 0.4)* .5 + .5)* 20 + 6 * (.7 + cos(3.14 / 10 * i * i * .01)); 
	
	//30 * (fabs(i - 25) < 10 || (fabs(i - 75) < 10));


    const float dz = tan(55 * M_PI / 180.);

    float o[N];//output

    const float a = - dz;
    float b = 0; //state, a x + b
    
    for(int x = 0; x < N; ++x)
    {
	const bool evalpred = h[x] > a * x + b;

	o[x] = evalpred;
	
	if (evalpred)
	    b = h[x] - a * x; 
    }

    for(int i = 0; i < N; ++i)
    	printf("%d\t%f\t%f\t%f\n", i, h[i], o[i], 50 + a * fmod(i, 50));
        
    return 0;
}
