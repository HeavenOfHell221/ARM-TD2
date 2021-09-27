#include "file_slider.h"

#include <QHBoxLayout>
#include <cmath>
#include <sstream>

FileSlider::FileSlider(const QString &slider_name, int file_nb, QWidget *parent)
    : QWidget(parent), file_nb(file_nb) {
  QSizePolicy size_policy;
  size_policy.setVerticalPolicy(QSizePolicy::Fixed);
  size_policy.setHorizontalPolicy(QSizePolicy::Minimum);
  setSizePolicy(size_policy);
  layout = new QHBoxLayout(this);
  name_label = new QLabel(slider_name);
  slider = new QSlider(Qt::Horizontal);
  slider->setMinimum(0);
  slider->setMaximum(file_nb);
  value_label = new QLabel("value");
  layout->addWidget(name_label);
  layout->addWidget(slider);
  layout->addWidget(value_label);

  connect(slider, SIGNAL(valueChanged(int)), this, SLOT(onSliderChanged(int)));
  updateValueLabel();
}

FileSlider::~FileSlider() {}

int FileSlider::value() { return slider->value(); }

void FileSlider::setValue(int new_value) {
  slider->setValue(new_value);
}

void FileSlider::setLimits(int file_nb) {
  file_nb = file_nb;
  slider->setValue(1);
  updateValueLabel();
}

void FileSlider::onSliderChanged(int new_value) {
  updateValueLabel();
  emit valueChanged(new_value);
}

void FileSlider::updateValueLabel() {
  std::ostringstream oss;
  oss << value() << "File number : " << file_nb;
  value_label->setText(QString(oss.str().c_str()));
}