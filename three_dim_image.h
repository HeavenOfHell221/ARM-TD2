#ifndef THREE_DIM_IMAGE_H
#define THREE_DIM_IMAGE_H

#include <vector>
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmimgle/dcmimage.h"

class ThreeDimImage {

    struct Voxel {
        float x, y, depth;
        unsigned char color;
    };

    public:
        ThreeDimImage();
        ~ThreeDimImage();
        void loadImages(std::vector<DcmFileFormat>& active_files);
    private:
        //std::vector<unsigned char*> _images;
        //unsigned char imageVector;
        std::vector<Voxel> _voxels;
};

#endif