#include "controller.h"

Controller::Controller(Model *model) : m(model) {}

void Controller::changeFile(QString &path) {
  m->path = path;
  m->loadOBJ();
  m->changeModelSendSignal();
}
