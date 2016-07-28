#include <cstdio>
#include <cmath>
#include <cassert>

#include <algorithm>
#include <vector>

void test(float alpha)
{
    alpha = (float)std::max(-0.25 * M_PI, std::min(0.25 * M_PI, (double)alpha));

    const float dx = cos(alpha);
    const float dy = sin(alpha);
    printf("direction: %f %f\n", dx, dy);

    const int NX = 188, NY = 234;

    std::vector<float> xpath, ypath;

    int iter = 0, xp, yp;

    //as dumb as it gets
    do
    {
	xp = round(iter * dx);
	yp = round(iter * dy);
	xpath.push_back(xp);	
	ypath.push_back(yp);

	++iter;
    }
    while (xp < NX && yp < NY);

    printf("path size: %d\n", (int)xpath.size());

    //somewhat smarter -- a-priori guess of the extent
    const int xcandidate = (int)1 + floor((NX + .4999) / fabs(dx));
    const int ycandidate = (int)1 + floor((NY + .4999) / fabs(dy));
    const int guessedsize = std::min(xcandidate, ycandidate);

    printf("guessed size: %d (%e %e)\n", guessedsize, .4999 + NX / fabs(dx), .4999 + NY / fabs(dy));
    assert(guessedsize == (int)xpath.size());
}

int main()
{
    //test(44 * M_PI / 180);

    for(int i = -45; i < 46; ++i)
    {
	printf("i = %d\n", i);
	test(i * M_PI / 180);
    }
 }
