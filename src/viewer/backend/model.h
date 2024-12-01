#ifndef MODEL_H
#define MODEL_H

#include <QtOpenGL>
#include <iostream>

namespace s21 {

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

 public:
  QVector<VertexData> triangles;
  QVector<GLuint> indices;
  QQuaternion rotation;
  QString path = "";
  QString texPath = "../../assets/textures/default.png";
  QString settPath = "../../assets/settings.conf";
  bool isWireframe = false, isDashed = false, showInfo = true,
       isParallel = false;
  int lineSize = 1, pointMode = 1, pointSize = 5;
  float scaleX = 0, scaleY = 0, scaleZ = 0;
  float translateX = 0, translateY = 0, translateZ = 0;
  float bgColor[3] = {0, 0, 0};
  float wireColor[3] = {255, 255, 255};
  float pointColor[3] = {255, 255, 255};
  float rotationX = 0, rotationY = 0, prevRotY = 0, prevRotX = 0;

  Model() { loadSettings(); }
  ~Model() { saveSettings(); }

  void loadSettings();
  void saveSettings();
  void loadOBJ();

 signals:
  void updateSignal();
  void uploadFileSignal();
  void repaintModelSignal();
};
}  // namespace s21

#endif  // MODEL_H
