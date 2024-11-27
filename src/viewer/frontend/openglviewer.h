#ifndef OpenGLViewer_H
#define OpenGLViewer_H

#include "../backend/controller.h"

class Viewer;

namespace s21 {
class OpenGLViewer : public QOpenGLWidget {
  friend class Viewer;
  Q_OBJECT

 public:
  QMatrix4x4 projectMatrix, modelViewMatrix;
  QOpenGLShaderProgram m_program;
  QOpenGLTexture *m_texture;
  QOpenGLBuffer m_arrayBuffer, m_indexBuffer;
  Model *model;
  QPointF mousePos;

  OpenGLViewer(QWidget *parent, Model *model);
  ~OpenGLViewer();
  void setTexture(QString filename);
  void genTexture(float red, float green, float blue);
  char *genFilename(QString name);
  void saveGIF(float x, float y, float z);
  std::vector<uint8_t> writeFrame(int width, int height, int channels);
  void setMode();
  void setMatrix();

 public slots:
  void saveIMG(int mode);
  void dataProcessing();
  void rotateModel(float angleX, float angleY, float angleZ);
  void scaleModel(float dirX, float dirY, float dirZ);
  void translateModel(float dirX, float dirY, float dirZ);

 private:
  void initializeGL();
  void resizeGL(int nWidth, int nHeight);
  void paintGL();
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void mouseWheelEvent(QWheelEvent *event);
  void initShaders();
};
}  // namespace s21
#endif  // OpenGLViewer_H
