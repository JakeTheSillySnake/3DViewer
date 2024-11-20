#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "model.h"

class Controller {
 public:
  Controller(Model *model);
  ~Controller() = default;
  void changeFile(QString &path);

  Model *m;
};

#endif  // CONTROLLER_H
