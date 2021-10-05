#include "three_dim_image.h"


ThreeDimImage::ThreeDimImage() {
    _voxels = std::vector<Voxel>();
}

void ThreeDimImage::loadImages(std::vector<DcmFileFormat>& active_files) {
    _voxels.clear();

    for(int i = 0; i < active_files.size(); i++) {
        DcmFileFormat *file = &active_files[i];

        DcmDataset *dataset = file->getDataset();
        
        if(dataset == nullptr) {
            // TODO
        }
        
        E_TransferSyntax wished_ts = EXS_LittleEndianExplicit;
        OFCondition status_ = dataset->chooseRepresentation(wished_ts, NULL);
        
        if(status_.bad()) {
            // TODO
        }

        DicomImage* img = new DicomImage(dataset, wished_ts);

        int bits_per_pixel = 8;
        unsigned long data_size = img->getOutputDataSize(bits_per_pixel);
        unsigned char* image_data = new unsigned char[data_size];
        int status = img->getOutputData((void *)image_data, data_size, bits_per_pixel);
        if (!status) {
            // TODO 
        }
        
        int width = img->getWidth();
        int height = img->getHeight();

        for(int x = 0; x < width; x++) 
        {
            for(int y = 0; y < height; y++) 
            {
                Voxel v_curr;
                v_curr.color = image_data[x + width * y];
                v_curr.x = ((x / (width-1.f)) - 1.f) * 2.f;
                v_curr.y = ((y / (height-1.f)) - 1.f) * 2.f;
                v_curr.depth = i;
                _voxels.push_back(v_curr);
            }
        }
        delete(img);
    }    
}
