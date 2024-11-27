#include <QApplication>

#include "frontend/openglviewer.h"
#include "frontend/viewer.h"

using namespace s21;

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  Model m;
  Controller c(&m);
  Viewer w(&c);
  w.setWindowIcon(
      QIcon(QFileInfo("../../assets/viewer_icon.png").absoluteFilePath()));
  w.setWindowTitle("Simple Viewer");
  w.show();
  return a.exec();
}
