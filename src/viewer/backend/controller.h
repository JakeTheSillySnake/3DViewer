#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "model.h"

namespace s21 {
class Controller : public QWidget {
  Q_OBJECT

 public:
  Model *model;

  Controller(Model *model);
  ~Controller(){};
  void changeFile(QString &path);

 public slots:
  void pointColor();
  void pointSize();
  void pointMode0();
  void pointMode1();
  void pointMode2();
  void thickness();
  void wireColor();
  void openTex();
  void defaultTex();
  void woodTex();
  void metalTex();
  void stoneTex();
  void waterTex();
  void changeBackground();
};
}  // namespace s21

#endif  // CONTROLLER_H
