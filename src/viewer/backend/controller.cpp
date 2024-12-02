#include "controller.h"

using namespace s21;

Controller::Controller(Model *model) : model(model) {}

void Controller::changeFile(QString &path) {
  model->loadOBJ(path);
  emit model->uploadFileSignal();
}

void Controller::pointColor(float red, float green, float blue) {
  model->pointColor[0] = red;
  model->pointColor[1] = green;
  model->pointColor[2] = blue;
  emit model->updateSignal();
}

void Controller::pointSize(int size) {
  model->pointSize = size;
  emit model->updateSignal();
}

void Controller::pointMode(int mode) {
  model->pointMode = mode;
  emit model->updateSignal();
}

void Controller::thickness(int size) {
  model->lineSize = size;
  emit model->updateSignal();
}

void Controller::wireColor(float red, float green, float blue) {
  model->wireColor[0] = red;
  model->wireColor[1] = green;
  model->wireColor[2] = blue;
  emit model->updateSignal();
}

void Controller::openTex(QString &path) {
  model->texPath = path;
  emit model->updateSignal();
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

void Controller::changeBackground(float red, float green, float blue) {
  model->bgColor[0] = red;
  model->bgColor[1] = green;
  model->bgColor[2] = blue;
  emit model->repaintModelSignal();
  emit model->updateSignal();
}

void Controller::rotateModel(float angleX, float angleY, float angleZ) {
  QVector3D axis[3] = {QVector3D(1.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0),
                       QVector3D(0.0, 0.0, 1.0)};
  float angle[3] = {angleX, angleY, angleZ};
  for (int i = 0; i < 3; i++) {
    model->rotation *= QQuaternion::fromAxisAndAngle(axis[i], angle[i]);
    emit model->updateSignal();
  }
}

void Controller::scaleModel(float dirX, float dirY, float dirZ) {
  if (model->scaleX >= -99 || dirX > 0) model->scaleX += dirX;
  if (model->scaleY >= -99 || dirY > 0) model->scaleY += dirY;
  if (model->scaleZ >= -99 || dirZ > 0) model->scaleZ += dirZ;
  emit model->updateSignal();
}

void Controller::translateModel(float dirX, float dirY, float dirZ) {
  model->translateX += dirX;
  model->translateY += dirY;
  model->translateZ += dirZ;
  emit model->updateSignal();
}
