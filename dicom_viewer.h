#ifndef DICOM_VIEWER_H
#define DICOM_VIEWER_H

#include <QMainWindow>
#include <QVBoxLayout>

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmimgle/dcmimage.h"

#include "double_slider.h"
#include "image_label.h"
#include "file_slider.h"
#include "point_cloud_display.h"
#include "three_dim_image.h"

class DicomViewer : public QMainWindow {
  Q_OBJECT

public:
  DicomViewer(QWidget *parent = 0);
  ~DicomViewer();

public slots:
  void openDicom();
  void showStats();
  void save();

  void onWindowCenterChange(double new_window_center);
  void onWindowWidthChange(double new_window_width);
  void onDisplayedFileChange(int new_displayed_file);

private:
  QWidget *widget;
  QGridLayout *layout;
  QHBoxLayout *img_layout;
  PointCloudDisplay *point_cloud;
  
  DoubleSlider *window_center_slider;
  DoubleSlider *window_width_slider;
  FileSlider *file_finder_slider;
  /// The area in which the image is shown
  ImageLabel *img_label;

  /// Collection des fichiers .dcm ouverts par l'utilisateur
  std::vector<DcmFileFormat> active_files;

  /// DicomImage actuellement affiché à l'écran
  DicomImage *curr_image;

  /// Dataset du fichier .dcm courant
  /// active_files[curr_file].getDataset();
  DcmDataset *curr_dataset;

  /// L'ID du fichier .dcm courant
  /// Utilisé comme ID dans la liste active_files pour récupérer le fichier .dcm
  int curr_file; 

  /// active_files.size()
  int getNumberActiveFiles();

  uchar *image_data;

  ThreeDimImage* images;

  DcmDataset *getDataset(int id);
  DicomImage *loadDicomImage(int id);

  void updateWindowSliders();
  void applyDefaultWindow();

  /// Adjust the number of files
  void updateDefaultFileSlider();

  /// Update the image based on current status of the object
  void updateImage();
  void displayImage(int id);

  std::string getPatientName();
  std::string getPatientName(int id);

  /// Convert current Dicom Image to a QImage according to actual parameters
  QImage getQImage();

  /// Extract min (and max) used (and allowed) values
  void getFrameMinMax(int id, double *min_used_value, double *max_used_value,
                 double *min_allowed_value = nullptr,
                 double *max_allowed_value = nullptr);

  void getCollectionMinMax(double *min_used_value, double *max_used_value, 
                          double *min_allowed_value, double *max_allowed_value);

  void getWindow(double *min_value, double *max_value);

  double getSlope();
  double getIntercept();

  double getWindowCenter();
  double getWindowWidth();
  double getWindowMin();
  double getWindowMax();

  std::string getInstanceNumber(int id);
  std::string getAcquisitionNumber(int id);

  int BinarySearch(std::vector<int> list, int value);
};

template <typename T> T getField(DcmItem *item, const DcmTagKey &tag_key);

template <typename T> T getField(DcmItem *item, unsigned int g, unsigned int e) {
  return getField<T>(item, DcmTagKey(g, e));
}

template <> double getField<double>(DcmItem *item, const DcmTagKey &tag_key);
template <> short int getField<short int>(DcmItem *item, const DcmTagKey &tag_key);
template <> std::string getField<std::string>(DcmItem *item, const DcmTagKey &tag_key);

#endif // DICOM_VIEWER_H
