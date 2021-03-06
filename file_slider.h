#ifndef FILE_SLIDER_H
#define FILE_SLIDER_H

#include <QLabel>
#include <QSlider>
#include <QWidget>

/// A class implementing a slider for navigate in differents files with:
/// - A name shown in a label
/// - An int slider
/// - The name of the file shown in a label

class FileSlider : public QWidget {
  Q_OBJECT
public:
  FileSlider(const QString &slider_name, int file_nb, QWidget *parent = 0);
  ~FileSlider();

  void setLimits(int file_nb);

  int value();

  void setValue(int new_value);

public slots:
  void onSliderChanged(int new_value);

signals:
  void valueChanged(int new_value);

private:
  QLayout *layout;
  QLabel *name_label;
  QSlider *slider;
  QLabel *value_label;

  int file_nb;

  void updateValueLabel();
};

#endif // FILE_SLIDER_H
