#ifndef THREE_DIM_IMAGE_H
#define THREE_DIM_IMAGE_H

#include <vector>
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmimgle/dcmimage.h"

class ThreeDimImage {
    public:
        ThreeDimImage();
        ~ThreeDimImage();
        void loadImages(std::vector<DcmFileFormat>& active_files);
    private:
        std::vector<unsigned char*> _images;
};

#endif