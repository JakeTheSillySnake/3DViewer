#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "model.h"

class Controller {
 public:
  Controller(Model *model);
  ~Controller() = default;
  void changeFile(QString &path);

  Model *m;
  OpenGLViewer *glWindow;

  /*private slots:
  void toggleDashed();
  void thickness();
  void wireColor();
  void toggleWireframe();
  void saveJPG();
  void saveBMP();
  void saveGIF();
  void openFile();
  void openTex();
  void defaultTex();
  void woodTex();
  void metalTex();
  void stoneTex();
  void waterTex();
  void rotate();
  void translate();
  void scale();
  void showInfo();
  void changeBackground();*/
};

#endif  // CONTROLLER_H
