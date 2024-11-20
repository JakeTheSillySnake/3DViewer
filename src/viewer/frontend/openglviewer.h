#ifndef OpenGLViewer_H
#define OpenGLViewer_H

#include "../backend/controller.h"

class Viewer;

class OpenGLViewer : public QOpenGLWidget {
  friend class Viewer;
  Q_OBJECT

 public:
  explicit OpenGLViewer(QWidget *parent, Model *model);
  void setVec3(float red, float green, float blue);
  void saveIMG(int mode);
  std::vector<uint8_t> writeFrame(int width, int height, int channels);
  void setTexture(QString filename);
  void saveGIF(float x, float y, float z);
  void genTexture(float red, float green, float blue);
  void setMode();
  void setMatrix();
  char *genFilename(QString name);
  ~OpenGLViewer();

 protected:
  virtual void initializeGL();
  virtual void resizeGL(int nWidth, int nHeight);
  virtual void paintGL();
  void initShaders();

 private:
  QMatrix4x4 projectMatrix;
  QVector<VertexData> vertexes;
  QVector<GLuint> indexes;
  QOpenGLShaderProgram m_program;
  QOpenGLTexture *m_texture;
  QOpenGLBuffer m_arrayBuffer;
  QOpenGLBuffer m_indexBuffer;
  QMatrix4x4 modelViewMatrix;
  Model *model;
  char typeProj = 'c';

 private slots:
  void dataProcessing();
  void rotateModelX(float angle);
  void rotateModelY(float angle);
  void rotateModelZ(float angle);
  void scaleModelX(int direction);
  void scaleModelY(int direction);
  void scaleModelZ(int direction);
  void translateModeleX(int direction);
  void translateModeleY(int direction);
  void translateModeleZ(int direction);
  void setProj();

 signals:
  void rotateModelSignalX(float angle);
  void rotateModelSignalY(float angle);
  void rotateModelSignalZ(float angle);
  void scaleModelSignalX(int direction);
  void scaleModelSignalY(int direction);
  void scaleModelSignalZ(int direction);
  void translateModeleSignalX(int direction);
  void translateModeleSignalY(int direction);
  void translateModeleSignalZ(int direction);
};

#endif  // OpenGLViewer_H
