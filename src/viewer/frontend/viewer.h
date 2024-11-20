#ifndef VIEWER_H
#define VIEWER_H

#include "../backend/controller.h"
#include "openglviewer.h"

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
  void pointColor();
  void pointSize();
  void pointMode0();
  void pointMode1();
  void pointMode2();
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
  void changeBackground();
  // controller end

  void keyPressEvent(QKeyEvent *event);

 private:
  Ui::Viewer *ui;
  QGraphicsView *view;
  QGraphicsScene *scene;
  QDockWidget *dock;
  QFont *font;
  QString filename;
  OpenGLViewer *glWindow;
  Controller *c;
  int edges;
  int vertices;
};
#endif  // VIEWER_H
