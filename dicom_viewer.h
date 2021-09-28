#ifndef DICOM_VIEWER_H
#define DICOM_VIEWER_H

#include <QMainWindow>
#include <QVBoxLayout>

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmimgle/dcmimage.h"

#include "double_slider.h"
#include "image_label.h"
#include "file_slider.h"

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
  QVBoxLayout *layout;
  DoubleSlider *window_center_slider;
  DoubleSlider *window_width_slider;
  FileSlider *file_finder_slider;
  /// The area in which the image is shown
  ImageLabel *img_label;

  /// The current DcmFileFormat
  std::vector<DcmFileFormat> active_files;

  /// The active Dicom image
  DicomImage *image;

  /// The 8-bits image to be shown on screen
  uchar *img_data;

  /// Current file number
  int curr_file;

  /// Retrieve access to the dataset of current file
  DcmDataset *getDataset();
  DcmDataset *getDataset(int id);

  /// Adjust the size of the window based on file content
  void updateWindowSliders();

  /// Load the DicomImage from the active slice
  /// If there are no active slice available, set image to nullptr
  void loadDicomImage();

  /// Import the default parameters from the DicomImage
  void applyDefaultWindow();

  /// Adjust the number of files
  void updateDefaultFileSlider();

  /// Update the image based on current status of the object
  void updateImage();

  /// Retrieve patient name from active file
  /// return 'FAIL' if no active file is found
  std::string getPatientName();
  std::string getPatientName(int id);

  /// Retrieve image from active file, converting to appropriate transfer syntax
  /// return nullptr on failure
  DicomImage *getDicomImage();

  /// Convert current Dicom Image to a QImage according to actual parameters
  QImage getQImage();

  /// Extract min (and max) used (and allowed) values
  void getMinMax(double *min_used_value, double *max_used_value,
                 double *min_allowed_value = nullptr,
                 double *max_allowed_value = nullptr);

  void getWindow(double *min_value, double *max_value);

  int getFileNb();

  double getSlope();
  double getIntercept();

  double getWindowCenter();
  double getWindowWidth();
  double getWindowMin();
  double getWindowMax();

  std::string getInstanceNumber();
  std::string getInstanceNumber(int id);
  std::string getAcquisitionNumber();
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
