#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "model.h"

namespace s21 {
class Controller : public QObject {
  Q_OBJECT

 public:
  Model *model;

  Controller(Model *model);
  ~Controller(){};

  void changeFile(QString &path);
  void pointColor(float red, float green, float blue);
  void pointSize(int size);
  void pointMode(int mode);
  void thickness(int size);
  void wireColor(float red, float green, float blue);
  void openTex(QString &path);
  void changeBackground(float red, float green, float blue);
  void rotateModel(float angleX, float angleY, float angleZ);
  void scaleModel(float dirX, float dirY, float dirZ);
  void translateModel(float dirX, float dirY, float dirZ);

 public slots:
  void defaultTex();
  void woodTex();
  void metalTex();
  void stoneTex();
  void waterTex();
};
}  // namespace s21

#endif  // CONTROLLER_H
