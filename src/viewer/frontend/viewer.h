#ifndef VIEWER_H
#define VIEWER_H

#include "../backend/controller.h"
#include "openglviewer.h"

using namespace s21;

QT_BEGIN_NAMESPACE
namespace Ui {
class Viewer;
}
QT_END_NAMESPACE

class Viewer : public QMainWindow {
  Q_OBJECT

 public:
  Viewer(Controller *controller, QWidget *parent = nullptr);
  ~Viewer();

  void loadFont();
  void viewSetup();
  void connectSlots();
  void drawField();
  void drawStats();
  void drawControls();
  std::pair<QStringList, bool> inputBox(QString action);
  void addText(QFont font, QGraphicsSimpleTextItem *text, int x, int y);
  void addButton(int x, int y, int width, int height, QString text);

 private slots:
  void errorMessage();
  void pointColor();
  void pointSize();
  void pointMode0();
  void pointMode1();
  void pointMode2();
  void thickness();
  void wireColor();
  void openTex();
  void toggleDashed();
  void toggleWireframe();
  void toggleProj();
  void changeBackground();
  void saveJPG();
  void saveBMP();
  void saveGIF();
  void openFile();
  void showInfo();
  void rotate();
  void translate();
  void scale();
  void keyPressEvent(QKeyEvent *event);

 private:
  Ui::Viewer *ui;
  QGraphicsView *view;
  QGraphicsScene *scene;
  QDockWidget *dock;
  QFont *font;
  QString filename;
  QErrorMessage *msg;
  OpenGLViewer *glWindow;
  Controller *c;
  int edges;
  int vertices;
};
#endif  // VIEWER_H
