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
  Controller *c;
  QPointF mousePos;

  OpenGLViewer(QWidget *parent, Controller *c);
  ~OpenGLViewer();

  void setTexture(QString filename);
  void genTexture(float red, float green, float blue);
  char *genFilename(QString name);
  void saveGIF(float x, float y, float z);
  std::vector<uint8_t> writeFrame(int width, int height, int channels);
  void setMode();
  void setMatrix();
  void saveIMG(int mode);

 public slots:
  void dataProcessing();

 private:
  void initializeGL();
  void resizeGL(int nWidth, int nHeight);
  void paintGL();
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);
  void initShaders();
};
}  // namespace s21
#endif  // OpenGLViewer_H
