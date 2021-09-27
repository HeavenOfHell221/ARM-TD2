#ifndef DOUBLE_SLIDER_H
#define DOUBLE_SLIDER_H

#include <QLabel>
#include <QSlider>
#include <QWidget>

class FileSlider : public QWidget {
  Q_OBJECT
public:
  FileSlider(const QString &slider_name, double min_val, double max_val,
               QWidget *parent = 0, int nb_steps = 1000);
  ~FileSlider();

  void setLimits(double min, double max);

  double value();

  void setValue(double new_value);

public slots:
  void onSliderChanged(int new_value);

signals:
  void valueChanged(double new_value);

private:
  QLayout *layout;
  QLabel *name_label;
  QSlider *slider;
  QLabel *value_label;

  double min_val;
  double max_val;

  void updateValueLabel();
  double innerToValue(int slider_value);
  int valueToInner(double value);
};

#endif // DOUBLE_SLIDER_H
