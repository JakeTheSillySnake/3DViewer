#include "controller.h"

using namespace s21;

Controller::Controller(Model *model) : model(model) {}

void Controller::changeFile(QString &path) {
  model->path = path;
  model->loadOBJ();
  emit model->uploadFileSignal();
}

void Controller::pointColor() {
  QColor color = QColorDialog::getColor(Qt::white, 0, "Choose Color");
  if (color.isValid()) {
    model->pointColor[0] = color.red();
    model->pointColor[1] = color.green();
    model->pointColor[2] = color.blue();
  }
  emit model->updateSignal();
}

void Controller::pointSize() {
  bool ok;
  int res = QInputDialog::getInt(0, "Point Size", "px", 1, 1, 15, 1, &ok);
  if (ok) model->pointSize = res;
  emit model->updateSignal();
}

void Controller::pointMode0() {
  model->pointMode = 0;
  emit model->updateSignal();
}

void Controller::pointMode1() {
  model->pointMode = 1;
  emit model->updateSignal();
}

void Controller::pointMode2() {
  model->pointMode = 2;
  emit model->updateSignal();
}

void Controller::thickness() {
  bool ok;
  int res = QInputDialog::getInt(0, "Line Width", "px", 1, 1, 20, 1, &ok);
  if (ok) model->lineSize = res;
  emit model->updateSignal();
  ;
}

void Controller::wireColor() {
  QColor color = QColorDialog::getColor(Qt::white, 0, "Choose Color");
  if (color.isValid()) {
    model->wireColor[0] = color.red();
    model->wireColor[1] = color.green();
    model->wireColor[2] = color.blue();
  }
  emit model->updateSignal();
}

void Controller::openTex() {
  QString filename =
      QFileDialog::getOpenFileName(0, tr("Open File"), QDir::currentPath(),
                                   tr("Images (*jpg *jpeg *bmp *png *tiff)"));
  if (!filename.isEmpty()) model->texPath = filename;
}

void Controller::defaultTex() {
  model->texPath = "../../assets/textures/default.png";
  emit model->updateSignal();
}

void Controller::woodTex() {
  model->texPath = "../../assets/textures/wood.png";
  emit model->updateSignal();
}

void Controller::metalTex() {
  model->texPath = "../../assets/textures/metal.png";
  emit model->updateSignal();
}

void Controller::stoneTex() {
  model->texPath = "../../assets/textures/stone.png";
  emit model->updateSignal();
}

void Controller::waterTex() {
  model->texPath = "../../assets/textures/water.png";
  emit model->updateSignal();
}

void Controller::changeBackground() {
  QColor color = QColorDialog::getColor(Qt::white, 0, "Choose Color");
  if (color.isValid()) model->bgColor[0] = color.red();
  model->bgColor[1] = color.green();
  model->bgColor[2] = color.blue();
  emit model->repaintModelSignal();
  emit model->updateSignal();
}
