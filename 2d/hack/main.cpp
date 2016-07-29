#include <cstdio>
#include <cassert>
#include <cinttypes>

#include <algorithm>
#include <vector>

#include <vtkImageData.h>
#include <vtkXMLImageDataWriter.h>

#include "kernels.h"

using namespace std;

void tovtk(const char path[],
	 float * data,
	 const int x0,
	 const int y0,
	 const int z0,
	 const int xsize,
	 const int ysize,
	 const int zsize)
{
    const char * dumprequest = getenv("VTKDUMP");

    if (dumprequest != 0 && strcmp(dumprequest, "0") == 0)
   	return;

    vtkImageData * imageData = vtkImageData::New();

    printf("VTK dimensions: %d %d %d\n", xsize, ysize, zsize);
    imageData->SetDimensions(xsize, ysize, zsize);
    imageData->SetOrigin(x0, y0, z0);
    imageData->AllocateScalars(VTK_FLOAT, 1);
    imageData->SetSpacing(1, 1, 1);

    float* pixel = static_cast<float*>(imageData->GetScalarPointer(0, 0, 0));

    const int nvoxels = xsize * ysize * zsize;

    memcpy(pixel, data, sizeof(*data) * nvoxels);

    vtkXMLImageDataWriter* writer = vtkXMLImageDataWriter::New();
    writer->SetFileName(path);

    writer->SetInputData(imageData);
    writer->Write();

    writer->Delete();
    imageData->Delete();
}

float * terrain(const int NX,
		const int NY)
{
    float * h = new float[NX * NY];

    memset(h, 0, sizeof(*h) * NX * NY);

    for(int i = 0; i < 12; ++i)
    {
	const int x0 = (int)(drand48() * (NX - 1));
	const int y0 = (int)(drand48() * (NY - 1));
	const int r = (int)(10 + drand48() * 20);

	for(int iy = max(0, y0 - r); iy <= min(y0 + r, NY - 1); ++iy)
	    for(int ix = max(0, x0 - r); ix <= min(x0 + r, NX - 1); ++ix)
	    {
		const bool pred = pow(ix - x0, 2) + pow(iy - y0, 2) <= pow(r, 2);

		if (pred)
		    h[ix + NX * iy] = 30;
	    }
    }

    return h;
}

void doit(const float dz,
	  const int xp[],
	  const int yp[],
	  const int np,
	  const float h[],
	  const int NX,
	  const int NY,
	  float output[])
{
    const float a = -dz;

    const int xpend = xp[np - 1];
    const int xpext = 1 + abs(xpend);

    const int M = xpext + NX; //number of states

    float * b = new float[M];
    memset(b, 0, sizeof(*b) * M);

    for(int ip = 0; ip < np; ++ip)
    {
	const int x0 = xp[ip];
	const int y0 = yp[ip];
	const float length = sqrt(pow((float)x0, 2) + pow((float)y0, 2));
	const bool write = ip == 0 || yp[ip - 1] != yp[ip];

	const float * hline = h + NX * y0;
	float * oline = output + NX * y0;

#if 1
	{
	    const int offset = x0 - max(0, xpend);

	    const int p0 = -offset;
	    const int p1 = NX - offset;
	    const int s = p0 < p1 ? p0 : p1;
	    const int e = p0 + p1 - s;
	    const int n = e - s;

	    const float bias = a * length;

	    const float * const __restrict__ hptr = hline + s + offset;
	    float * const __restrict__ optr = oline + s + offset;
	    float * const __restrict__ sptr = b + s;

	    scan_kernel(write, n, a * length, hptr, optr, sptr);
	}
#else
	int ctr = 0;

	for(int j = 0; j < M; ++j)
	{
	    const int x = j + x0 - max(0, xpend);

	    if (x >= 0 && x < NX)
	    {
		++ctr;
		const bool evalpred = hline[x] > a * length + b[j];

		if (write)
		    oline[x] = evalpred;//if we output "j/10" we'll see the rays

		if (evalpred)
		    b[j] = hline[x] - a * length ;
    	    }
	}

	assert(ctr == NX);
#endif
    }

    delete [] b;
}

uint64_t rdtsc() 
{
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

int main()
{
    const int NX = 423, NY = 288;

    float * h = terrain(NX, NY);
    tovtk("terrain.vti", h, 0, 0, 0, NX, NY, 1);

    float alpha = (90 + 45) * M_PI / 180.;
    float gamma = 65 * M_PI / 180.;

    alpha = (float)max(0.25 * M_PI, min(0.75 * M_PI, (double)alpha));

    const float dx = cos(alpha);
    const float dy = sin(alpha);
    const float dz = tan(gamma);

    std::vector<int> xpath, ypath;

    {
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
    }

    float * output = new float[NX * NY];
    memset(output, 0xff, NX * NY * sizeof(float));

    bool profile = true;

    if (profile)
    {
	const int64_t cycles_start = rdtsc();

	const int ntimes = 100;
	for(int i = 0; i < 100; ++i)
	    doit(dz, &xpath.front(), &ypath.front(), -1 + (int)xpath.size(),
		 h, NX, NY, output);

	const int64_t cycles_end = rdtsc();

	const size_t fpall = (NX * NY * 2 + /*reading and writing the states */2 * NX * NY) * sizeof(float);
	const size_t fpout = (NX * NY) * sizeof(float);
	const double cycles = (cycles_end - cycles_start) * 1. / ntimes;
	printf("\x1b[92mTTS: %.2e\n"
	       "BW: %.2f B/C\n"
	       "TH: %.2f B/C\x1b[0m\n",
	       cycles, fpall / cycles, fpout / cycles);

	memset(output, 0xff, NX * NY * sizeof(float));
    }
    
    doit(dz, &xpath.front(), &ypath.front(), -1 + (int)xpath.size(),
	 h, NX, NY, output);

    tovtk("output.vti", output, 0, 0, 0, NX, NY, 1);

    delete [] output;
    delete [] h;

    return 0;
}
