#include "three_dim_image.h"


ThreeDimImage::ThreeDimImage() {
    _images = std::vector<unsigned char*>();
}

void ThreeDimImage::loadImages(std::vector<DcmFileFormat>& active_files) {
    _images.clear();
    for(auto file : active_files) {
        DcmDataset *dataset = file.getDataset();
        
        if(dataset == nullptr) {
            // TODO
        }
        
        E_TransferSyntax wished_ts = EXS_LittleEndianExplicit;
        OFCondition status_ = dataset->chooseRepresentation(wished_ts, NULL);
        
        if(status_.bad()) {
            // TODO
        }

        DicomImage *img = new DicomImage(dataset, wished_ts);

        int bits_per_pixel = 8;
        unsigned long data_size = img->getOutputDataSize(bits_per_pixel);
        unsigned char* image_data = new unsigned char[data_size];
        int status = img->getOutputData((void *)image_data, data_size, bits_per_pixel);
        if (!status) {
            // TODO 
        }
        _images.push_back(image_data);
    }    
}