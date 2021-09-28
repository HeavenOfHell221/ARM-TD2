#include "dicom_viewer.h"

#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>

#include "dcmtk/dcmdata/dcrledrg.h"
#include "dcmtk/dcmjpeg/djdecode.h"

DicomViewer::DicomViewer(QWidget *parent)
    : QMainWindow(parent), curr_image(nullptr), curr_dataset(nullptr), image_data(nullptr) {
  // Setting layout
  widget = new QWidget();
  setCentralWidget(widget);
  img_label = new ImageLabel(this);
  img_label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  img_label->setMinimumSize(200,200);
  img_label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  layout = new QVBoxLayout();
  // Using default values for limits, they are updated anyway once a file is loaded
  window_center_slider = new DoubleSlider("Window center", -1000.0, 1000.0);
  window_width_slider = new DoubleSlider("Window width", 1.0, 5000.0);
  file_finder_slider = new FileSlider("Current file", 1);
  layout->addWidget(window_center_slider);
  layout->addWidget(window_width_slider);
  layout->addWidget(file_finder_slider);
  file_finder_slider->setVisible(false); // only one file
  layout->addWidget(img_label);
  widget->setLayout(layout);
  // Setting menu
  QMenu *file_menu = menuBar()->addMenu("&File");
  QAction *open_action = file_menu->addAction("&Open");
  open_action->setShortcut(QKeySequence::Open);
  QObject::connect(open_action, SIGNAL(triggered()), this, SLOT(openDicom()));
  QAction *save_action = file_menu->addAction("&Save");
  save_action->setShortcut(QKeySequence::Save);
  QObject::connect(save_action, SIGNAL(triggered()), this, SLOT(save()));
  QAction *help_action = file_menu->addAction("&Help");
  help_action->setShortcut(QKeySequence::HelpContents);
  QObject::connect(help_action, SIGNAL(triggered()), this, SLOT(showStats()));
  // Sliders connection
  connect(window_center_slider, SIGNAL(valueChanged(double)), this, SLOT(onWindowCenterChange(double)));
  connect(window_width_slider, SIGNAL(valueChanged(double)), this, SLOT(onWindowWidthChange(double)));
  connect(file_finder_slider, SIGNAL(valueChanged(int)), this, SLOT(onDisplayedFileChange(int)));
  DcmRLEDecoderRegistration::registerCodecs();
  DJDecoderRegistration::registerCodecs();
}

DicomViewer::~DicomViewer() {}

void DicomViewer::openDicom() {
  QStringList fileNames = QFileDialog::getOpenFileNames(this, "Select one (or several) file(s) to open", ".", "DICOM (*.dcm)");

  // TODO : Tester si la liste fileNames n'est pas vide
  // Auquel cas return

  active_files.clear();
  curr_file = 0;
  std::vector<int> ids;
  std::string patientName;

  for (int i = 0; i < fileNames.length(); i++) {
    std::string path = fileNames[i].toStdString();
    OFCondition status;
    active_files.push_back(DcmFileFormat());
    status = active_files[i].loadFile(path.c_str());
    if (status.bad()) {
      QMessageBox::critical(this, "Failed to open file", path.c_str());
      return;
    }

    if (i == 0)
        patientName = getPatientName(0);
    else if (patientName != getPatientName(i)) {
        std::cerr << "File #" << ids[i] << " (" << path << ") doesn't have the same patient!" << std::endl;
        return;
    }

    int val = atoi(getInstanceNumber(i).c_str());
    int index = BinarySearch(ids, val);
    ids.insert(ids.begin() + index, val);
  }

  for (int i = 1; i < fileNames.length() - 1; i++) {
      if (ids[i] == ids[i - 1] || ids[i] == ids[i + 1]) {
          std::cerr << "Duplicate element #" << ids[i] << std::endl;
          return;
      } else if (ids[i] != ids[i - 1] + 1) {
          std::cerr << "Missing element #" << ids[i] + 1 << " (we have #" << ids[i] << ")" << std::endl;
          return;
      } else if (ids[i] != ids[i + 1] - 1) {
          std::cerr << "Missing element #" << ids[i] << " (we have #" << ids[i] - 1 << ")" << std::endl;
          return;
      }
  }

  if (curr_image != nullptr)
    delete (curr_image);

  if(curr_dataset != nullptr)
    delete (curr_dataset);
  
  curr_image = loadDicomImage(curr_file);
  curr_dataset = getDataset(curr_file);
  updateWindowSliders();
  updateDefaultFileSlider();
  applyDefaultWindow();
  updateImage();
}

int DicomViewer::BinarySearch(std::vector<int> list, int value) {
    int min = 0;
    int max = list.size() - 1;
    while (min <= max) {
        int v = floor((min + max) / 2);
        if (list[v] < value)      min = v + 1;
        else if (list[v] > value) max = v - 1;
        else                      return v;
    }
    return min;
}

void DicomViewer::save() {
  QString fileName = QFileDialog::getSaveFileName(
      this, tr("Save image to: "), "tmp.png", tr("Images (*.png *.xpm *.jpg)"));
  if (!getQImage().save(fileName))
    QMessageBox::critical(this, "Failed to save file", fileName);
}

void DicomViewer::showStats() {
  std::ostringstream msg_oss;

  /*

    TODO Gabriel :

    - Number of slices -> getNumberSlices();
    - Current frame min/max values used -> getFrameMinMax(...)
    - Collection min/max values used -> getCollectionMinMax(...)
  */
  msg_oss << "----- Global information -----" << std::endl;
  msg_oss << "Number of slices: " << getNumberActiveFiles() << std::endl;

  double min_used_value, max_used_value, min_allowed_value, max_allowed_value;
  getCollectionMinMax(&min_used_value, &max_used_value, &min_allowed_value, &max_allowed_value);
  msg_oss << "Extremum collection allowed values: [" << min_allowed_value << ", " << max_allowed_value << "]" << std::endl;
  msg_oss << "Extremum collection used values: [" << min_used_value << ", " << max_used_value << "]" << std::endl;

  msg_oss << "\n----- Current slice information -----" << std::endl;
  msg_oss << "Patient: " << getPatientName() << std::endl;
  msg_oss << "Instance number: " << getInstanceNumber() << std::endl;
  msg_oss << "Acquisition number: " << getAcquisitionNumber() << std::endl;
  E_TransferSyntax original_syntax = curr_dataset->getOriginalXfer();
  DcmXfer xfer(original_syntax);
  msg_oss << "Original transfer syntax: (" << original_syntax << ") " << xfer.getXferName() << std::endl;
  DicomImage *image = curr_image;
  if (image) {
    msg_oss << "Nb frames: " << image->getFrameCount() << std::endl;
    msg_oss << "Size: " << image->getWidth() << "*" << image->getHeight() << "*" << image->getDepth() << std::endl;
    double min_used_value, max_used_value, min_allowed_value, max_allowed_value;
    getFrameMinMax(curr_file, &min_used_value, &max_used_value, &min_allowed_value, &max_allowed_value);
    msg_oss << "Allowed values: [" << min_allowed_value << ", " << max_allowed_value << "]" << std::endl;
    msg_oss << "Used values: [" << min_used_value << ", " << max_used_value << "]" << std::endl;
    msg_oss << "Window: [" << getWindowMin() << ", " << getWindowMax() << "]" << std::endl;
    msg_oss << "Slope: " << getSlope() << std::endl;
    msg_oss << "Intercept: " << getIntercept() << std::endl;
  } else {
    msg_oss << "No available image" << std::endl;
  }
  QMessageBox::information(this, "DCM file properties", msg_oss.str().c_str());
}

void DicomViewer::onWindowCenterChange(double new_window_center) {
  (void)new_window_center;
  updateImage();
}

void DicomViewer::onWindowWidthChange(double new_window_width) {
  (void)new_window_width;
  updateImage();
}
void DicomViewer::onDisplayedFileChange(int new_displayed_file) {
  curr_file = new_displayed_file - 1;
  loadDicomImage();
  updateImage();
}

void DicomViewer::updateWindowSliders() {
  double min_used_value, max_used_value;
  getFrameMinMax(curr_file, &min_used_value, &max_used_value);
  window_center_slider->setLimits(min_used_value, max_used_value);
  window_width_slider->setLimits(1.0, max_used_value - min_used_value);
}

void DicomViewer::updateDefaultFileSlider() {
  int file_number = getFileNb();
  std::cout << "File number: " << active_files.size() << std::endl;
  file_finder_slider->setVisible(file_number > 1);
  file_finder_slider->setLimits(file_number);
}

DicomImage *DicomViewer::loadDicomImage(int id) {
  DcmDataset *dataset = getDataset(id);
  DicomImage *img = nullptr;
  if (dataset == nullptr) {
    return img;
  }
  // Changing syntax to a common one
  E_TransferSyntax wished_ts = EXS_LittleEndianExplicit;
  OFCondition status = dataset->chooseRepresentation(wished_ts, NULL);
  if (status.bad()) {
    QMessageBox::critical(this, "Dicom Image failure", status.text());
    return img;
  }
  return new DicomImage(dataset, wished_ts);
}

DcmDataset *DicomViewer::getDataset(int id) {
  return active_files[id].getDataset();
}

void DicomViewer::applyDefaultWindow() {
  window_center_slider->setValue(getWindowCenter());
  window_width_slider->setValue(getWindowWidth());
}

void DicomViewer::updateImage() {
  if (curr_image == nullptr) {
    img_label->setText("No available image");
    return;
  }

  // Set window
  double window_center = window_center_slider->value();
  double window_width = window_width_slider->value();
  curr_image->setWindow(window_center, window_width);
  img_label->setImg(getQImage());
}

std::string DicomViewer::getPatientName() {
  return getPatientName(curr_file);
}

std::string DicomViewer::getPatientName(int id) {
  return getField<std::string>(getDataset(id), DCM_PatientName); 
}

int DicomViewer::getNumberActiveFiles() {
  return active_files.size();
}

QImage DicomViewer::getQImage() {
  DicomImage *dicom = curr_image;
  int bits_per_pixel = 8;
  unsigned long data_size = dicom->getOutputDataSize(bits_per_pixel);
  if (image_data != nullptr) {
    free(image_data);
  }
  image_data = new unsigned char[data_size];
  int status = dicom->getOutputData((void *)image_data, data_size, bits_per_pixel);
  if (!status) {
    QMessageBox::critical(this, "Fatal error",
                          "Failed to get output data when getting QImage");
    return QImage();
  }
  int width = dicom->getWidth();
  int height = dicom->getHeight();

  return QImage(image_data, width, height, QImage::Format_Grayscale8);
}

void DicomViewer::getCollectionMinMax(double *min_used_value, double *max_used_value, double *min_allowed_value, double *max_allowed_value) {
  int N = getNumberActiveFiles();
  
  double min_used, max_used, min_allowed, max_allowed;

  for(int id = 0; id < N; id++) {
    DicomImage *img = loadDicomImage(id);  
    img->getMinMaxValues(min_used, max_used, 0);
    img->getMinMaxValues(min_allowed, max_allowed, 1);
    delete(img);
    if(id == 0) {
      *min_used_value = min_used;
      *max_used_value = max_used;
      *min_allowed_value = min_allowed;
      *max_allowed_value = max_allowed;
    }
    else {
      if(min_used < *min_used_value)
        *min_used_value = min_used;
      if(max_used > *max_used_value)
        *max_used_value = max_used;
      if(min_allowed < *min_allowed_value)
        *min_allowed_value = min_allowed;
      if(max_allowed > *max_allowed_value)
        *max_allowed_value = max_allowed;
    }
  }
}

void DicomViewer::getFrameMinMax(int id, double *min_used_value, double *max_used_value, double *min_allowed_value, double *max_allowed_value) {
  DicomImage *img = loadDicomImage(id);  
  if(img == nullptr) {
    // TODO ERROR
    return;
  } 
  img->getMinMaxValues(*min_used_value, *max_used_value, 0);
  if (min_allowed_value != nullptr || max_allowed_value != nullptr) {
    double min_allowed, max_allowed;
    img->getMinMaxValues(min_allowed, max_allowed, 1);
    if (min_allowed_value)
      *min_allowed_value = min_allowed;
    if (max_allowed_value)
      *max_allowed_value = max_allowed;
  }
  delete(img);
}

void DicomViewer::getWindow(double *min_value, double *max_value) {
  double center(0), width(0);
  curr_image->getWindow(center, width);
  *min_value = center - width / 2;
  *max_value = center + width / 2;
}

int DicomViewer::getFileNb() {
  return active_files.size();
}

double DicomViewer::getSlope() {
  return getField<double>(curr_dataset, DcmTagKey(0x28, 0x1053));
}

double DicomViewer::getIntercept() {
  return getField<double>(curr_dataset, DcmTagKey(0x28, 0x1052));
}

double DicomViewer::getWindowCenter() {
  return getField<double>(curr_dataset, DcmTagKey(0x28, 0x1050));
}

double DicomViewer::getWindowWidth() {
  return getField<double>(curr_dataset, DcmTagKey(0x28, 0x1051));
}

double DicomViewer::getWindowMin() {
  return getWindowCenter() - getWindowWidth() / 2;
}
double DicomViewer::getWindowMax() {
  return getWindowCenter() + getWindowWidth() / 2;
}

std::string DicomViewer::getInstanceNumber() { return getInstanceNumber(curr_file); }
std::string DicomViewer::getInstanceNumber(int id) { return getField<std::string>(getDataset(id), 0x20, 0x13); }

std::string DicomViewer::getAcquisitionNumber() { return getAcquisitionNumber(curr_file); }
std::string DicomViewer::getAcquisitionNumber(int id) { return getField<std::string>(getDataset(id), 0x20, 0x12); }

template <> double getField<double>(DcmItem *item, const DcmTagKey &tag_key) {
  double value;
  OFCondition status = item->findAndGetFloat64(tag_key, value);
  if (status.bad())
    std::cerr << "Error on tag: " << tag_key << " -> " << status.text() << std::endl;
  return value;
}

template <> short int getField<short int>(DcmItem *item, const DcmTagKey &tag_key) {
  short int value;
  OFCondition status = item->findAndGetSint16(tag_key, value);
  if (status.bad())
    std::cerr << "Error on tag: " << tag_key << " -> " << status.text() << std::endl;
  return value;
}

template <> std::string getField<std::string>(DcmItem *item, const DcmTagKey &tag_key) {
  OFString value;
  OFCondition status = item->findAndGetOFStringArray(tag_key, value);
  if (status.bad())
    std::cerr << "Error on tag: " << tag_key << " -> " << status.text() << std::endl;
  return value.c_str();
}
