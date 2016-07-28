#include <cstdio>

#include <algorithm>

#include <vtkImageData.h>
#include <vtkXMLImageDataWriter.h>

using namespace std;

void vtkdump(const char path[],
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


int main()
{
    const int NX = 288, NY = 423;

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
    
   
    vtkdump("test.vti", h, 0, 0, 0, NX, NY, 1);
    
    delete [] h;
    return 0;
}
