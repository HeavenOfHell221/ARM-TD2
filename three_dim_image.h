#ifndef THREE_DIM_IMAGE_H
#define THREE_DIM_IMAGE_H

#include <vector>
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmimgle/dcmimage.h"

struct Voxel {
    double x, y, z;
    float color;
};

class ThreeDimImage {
    public:
        ThreeDimImage();
        ~ThreeDimImage();
        void loadImages(std::vector<DcmFileFormat>& active_files);
        std::vector<Voxel> GetVoxels();
    private:
        //std::vector<unsigned char*> _images;
        //unsigned char imageVector;
        std::vector<Voxel> _voxels;
};

#endif
