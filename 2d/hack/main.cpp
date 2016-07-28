#include <cstdio>
#include <cassert>

#include <algorithm>
#include <vector>

#include <vtkImageData.h>
#include <vtkXMLImageDataWriter.h>

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


float * terrain(const int NX, const int NY)
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
    
    const int xpmin = *min_element(xp, xp + np);
    const int xpmax = *max_element(xp, xp + np);
    
    const int xpext = max(xpmax, -xpmin);
    printf("xpath extent: %d [%d %d]\n", xpext, xpmin, xpmax);
    assert(xpext > 0);

    const int M = xpext + NX + 1;
    printf("total dofs: %d\n", M);
    
    float * b = new float[M];
    memset(b, 0, sizeof(*b) * M);

    for(int ip = 0; ip < np; ++ip)
    {
	const int x0 = xp[ip]; 
	const int y0 = yp[ip];

	bool writable = ip == 0 || yp[ip - 1] != yp[ip];
	
	printf("x0: %d y0: %d\n", x0, y0);
	
	const float length = sqrt(pow((float)x0, 2) + pow((float)y0, 2));

	const float * hline = h + NX * y0;
	float * oline = output + NX * y0;

	int ctr = 0;
	//printf("x0: %d-> %d\n", x0, x0 - xpext);
	for(int j = 0; j < M; ++j)
	{
	    const int x = j + x0 - xpext;
	    
	    if (x >= 0 && x < NX)
	    {
		++ctr;
		const bool evalpred = hline[x] > a * length + b[j];

		if (writable)
		    oline[x] = evalpred;//j/10;// evalpred;
	
		if (evalpred)
		    b[j] = hline[x] - a * length ;
    	    }
	}
	assert(ctr == NX);
	//printf("ctr: %d\n", ctr);
	//exit(0);
    }
    
    delete [] b;
}

int main()
{
    const int NX = 423, NY = 288;

    float * h = terrain(NX, NY);
    tovtk("terrain.vti", h, 0, 0, 0, NX, NY, 1);
    
    
    float alpha = (90 - 35) * M_PI / 180.;
    float gamma = 15 * M_PI / 180.;

    //alpha = (float)max(-0.25 * M_PI, min(0.25 * M_PI, (double)alpha));

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

    doit(dz, &xpath.front(), &ypath.front(), -1 + (int)xpath.size(),
	 h, NX, NY, output); 

    tovtk("output.vti", output, 0, 0, 0, NX, NY, 1);
    delete [] output;
    delete [] h;

    return 0;
}