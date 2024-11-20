#ifndef MODEL_H
#define MODEL_H

#include <QtOpenGL>
#include <iostream>

class OpenGLViewer;
class Controller;

struct VertexData {
  VertexData() {}
  VertexData(QVector3D &p, QVector2D &t, QVector3D &n)
      : position(p), texCoord(t), normals(n) {}
  QVector3D position;
  QVector2D texCoord;
  QVector3D normals;
} typedef VertexData;

class Model : public QObject {
  Q_OBJECT
  friend class OpenGLViewer;
  friend class Controller;

 signals:
  void changeFileSignal();

 public:
  // Settings (default)
  QQuaternion rotation;
  QString path = "";
  QString texPath = "../../assets/textures/default.png";
  char *settPath = "../../assets/settings.conf";
  float bgColor[3] = {0, 0, 0}, wireColor[3] = {255, 255, 255}, pointColor[3] = {255, 255, 255};
  bool isWireframe = false, isDashed = false, showInfo = true;
  int lineSize = 1, pointMode = 1, pointSize = 5;
  int scaleX = 0, scaleY = 0, scaleZ = 0;
  int translateX = 0, translateY = 0, translateZ = 0;

  Model() { loadSettings(); }
  ~Model() { saveSettings(); }
  void changeModelSendSignal();
  void loadSettings();
  void saveSettings();
  void loadOBJ();

 private:
  QVector<QString> fileLines;
};

#endif  // MODEL_H
