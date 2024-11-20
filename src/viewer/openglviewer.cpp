#include "openglviewer.h"

#include "../external/gif.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION  // only where it's used
#include "../external/stb_image_write.h"

OpenGLViewer::OpenGLViewer(QWidget *parent, Model *model)
    : QOpenGLWidget{parent},
      m_texture(0),
      m_indexBuffer(QOpenGLBuffer::IndexBuffer),
      model(model) {
  connect(model, SIGNAL(changeFileSignal()), this, SLOT(dataProcessing()));
  connect(this, SIGNAL(rotateModelSignalX(float)), this,
          SLOT(rotateModelX(float)));
  connect(this, SIGNAL(rotateModelSignalY(float)), this,
          SLOT(rotateModelY(float)));
  connect(this, SIGNAL(rotateModelSignalZ(float)), this,
          SLOT(rotateModelZ(float)));
  connect(this, SIGNAL(scaleModelSignalX(int)), this, SLOT(scaleModelX(int)));
  connect(this, SIGNAL(scaleModelSignalY(int)), this, SLOT(scaleModelY(int)));
  connect(this, SIGNAL(scaleModelSignalZ(int)), this, SLOT(scaleModelZ(int)));
  connect(this, SIGNAL(translateModeleSignalX(int)), this,
          SLOT(translateModeleX(int)));
  connect(this, SIGNAL(translateModeleSignalY(int)), this,
          SLOT(translateModeleY(int)));
  connect(this, SIGNAL(translateModeleSignalZ(int)), this,
          SLOT(translateModeleZ(int)));
}

void OpenGLViewer::initializeGL() {
  glClearColor(model->bgColor[0] / 255.0, model->bgColor[1] / 255.0,
               model->bgColor[2] / 255.0, 1.0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  initShaders();
  dataProcessing();
}

void OpenGLViewer::resizeGL(int nWidth, int nHeight) {
  float aspect = nWidth / (float)nHeight;
  projectMatrix.setToIdentity();
  if (typeProj == 'c')
    projectMatrix.perspective(45, aspect, 0.1f, 800.0f);
  else if (typeProj == 'p')
    projectMatrix.ortho(-4.0f, 4.0f, -4.0f, 4.0f, 0.1f, 800.0f);
}

void OpenGLViewer::paintGL() {
  resizeGL(this->width(), this->height());
  setMode();
  setMatrix();
  m_arrayBuffer.bind();
  int vertLoc = m_program.attributeLocation("qt_Vertex");
  m_program.enableAttributeArray(vertLoc);
  m_program.setAttributeBuffer(vertLoc, GL_FLOAT, 0, 3, sizeof(VertexData));
  int textLoc = m_program.attributeLocation("qt_MultiTexCoord0");
  m_program.enableAttributeArray(textLoc);
  m_program.setAttributeBuffer(textLoc, GL_FLOAT, sizeof(QVector3D), 2,
                               sizeof(VertexData));
  m_indexBuffer.bind();
  glLineWidth(model->lineSize);
  glPointSize(model->pointSize);
  if (model->isWireframe && model->pointMode) {
      genTexture(model->pointColor[0], model->pointColor[1], model->pointColor[2]);
      glDrawElements(GL_TRIANGLES, m_indexBuffer.size(), GL_UNSIGNED_INT, 0);
    }
  if (model->isWireframe) {
      genTexture(model->wireColor[0], model->wireColor[1], model->wireColor[2]);
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
  glDrawElements(GL_TRIANGLES, m_indexBuffer.size(), GL_UNSIGNED_INT, 0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void OpenGLViewer::setMode() {
  m_texture->bind(0);
  if (model->isDashed && model->isWireframe) {
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(4, 0xAAAA);
  } else {
    glDisable(GL_LINE_STIPPLE);
  }
  if (model->isWireframe && model->pointMode) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    if (model->pointMode == 2) {
      glEnable(GL_POINT_SMOOTH);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
      glDisable(GL_POINT_SMOOTH);
      glDisable(GL_BLEND);
      }
  }
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(model->bgColor[0] / 255.0, model->bgColor[1] / 255.0,
               model->bgColor[2] / 255.0, 1.0f);
}

void OpenGLViewer::setMatrix() {
  modelViewMatrix.setToIdentity();
  modelViewMatrix.translate(0.01 * model->translateX, 0.01 * model->translateY,
                            -5.0 + 0.01 * model->translateZ);
  modelViewMatrix.rotate(model->rotation);
  QMatrix4x4 scalingMatrix;
  scalingMatrix.scale(1.0f + 0.01 * model->scaleX, 1.0f + 0.01 * model->scaleY,
                      1.0f + 0.01 * model->scaleZ);
  modelViewMatrix *= scalingMatrix;
  m_program.setUniformValue("qt_ModelViewProjectionMatrix",
                            projectMatrix * modelViewMatrix);
  m_program.setUniformValue("qt_Texture0", 0);
}

void OpenGLViewer::dataProcessing() {
  QVector<QVector3D> vertices, normals;
  QVector<QVector2D> uvs;
  vertexes.clear();
  indexes.clear();
  if (m_arrayBuffer.isCreated()) m_arrayBuffer.destroy();
  if (m_indexBuffer.isCreated()) m_indexBuffer.destroy();
  for (int i = 0; i < model->fileLines.size(); i++) {
    QString str = model->fileLines[i];
    QStringList list = str.split(" ");
    if (list[0] == "v")
      vertices.append(
          QVector3D(list[1].toFloat(), list[2].toFloat(), list[3].toFloat()));
    else if (list[0] == "vt")
      uvs.append(QVector2D(list[1].toFloat(), list[2].toFloat()));
    else if (list[0] == "vn")
      normals.append(
          QVector3D(list[1].toFloat(), list[2].toFloat(), list[3].toFloat()));
    else if (list[0] == "f") {
      for (int i = 1; i <= 3; i++) {
        QStringList innerList = list[i].split("/");
        vertexes.append(VertexData(vertices[innerList[0].toInt() - 1],
                                   uvs[innerList[1].toInt() - 1],
                                   normals[innerList[2].toInt() - 1]));
        indexes.append(indexes.size());
      }
    }
  }
  m_arrayBuffer.create();
  m_arrayBuffer.bind();
  m_arrayBuffer.allocate(vertexes.constData(),
                         vertexes.size() * sizeof(VertexData));
  m_indexBuffer.create();
  m_indexBuffer.bind();
  m_indexBuffer.allocate(indexes.constData(), indexes.size() * sizeof(GLuint));
  repaint();
}

void OpenGLViewer::genTexture(float red, float green, float blue) {
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  unsigned char *data = new unsigned char[3];
  data[0] = (unsigned char)red;
  data[1] = (unsigned char)green;
  data[2] = (unsigned char)blue;
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE,
               data);
  delete[] data;
}

void OpenGLViewer::setTexture(QString filename) {
  delete m_texture;
  m_texture = new QOpenGLTexture(
      QImage(QFileInfo(filename).absoluteFilePath()).mirrored());
  model->texPath = filename;
  repaint();
}

void OpenGLViewer::initShaders() {
  QFile tex = QFileInfo(model->texPath).absoluteFilePath();
  if (!tex.exists()) model->texPath = "../assets/textures/default.png";
  m_texture = new QOpenGLTexture(
      QImage(QFileInfo(model->texPath).absoluteFilePath()).mirrored());
  m_program.addShaderFromSourceFile(
      QOpenGLShader::Vertex,
      QFileInfo("../assets/vshader.vsh").absoluteFilePath());
  m_program.addShaderFromSourceFile(
      QOpenGLShader::Fragment,
      QFileInfo("../assets/fshader.fsh").absoluteFilePath());
  m_program.link();
  m_program.bind();
}

OpenGLViewer::~OpenGLViewer() {
  delete m_texture;
  m_indexBuffer.destroy();
  m_arrayBuffer.destroy();
}

void OpenGLViewer::setVec3(float red, float green, float blue) {
  model->bgColor[0] = red;
  model->bgColor[1] = green;
  model->bgColor[2] = blue;
}

void OpenGLViewer::rotateModelX(float angle) {
  QVector3D axis = QVector3D(1.0, 0.0, 0.0);
  model->rotation =
      QQuaternion::fromAxisAndAngle(axis, angle) * model->rotation;
  update();
}

void OpenGLViewer::rotateModelY(float angle) {
  QVector3D axis = QVector3D(0.0, 1.0, 0.0);
  model->rotation =
      QQuaternion::fromAxisAndAngle(axis, angle) * model->rotation;
  update();
}

void OpenGLViewer::rotateModelZ(float angle) {
  QVector3D axis = QVector3D(0.0, 0.0, 1.0);
  model->rotation =
      QQuaternion::fromAxisAndAngle(axis, angle) * model->rotation;
  update();
}

void OpenGLViewer::scaleModelX(int direction) {
  if (model->scaleX >= -99 || direction > 0) {
    model->scaleX += direction;
    update();
  }
}

void OpenGLViewer::scaleModelY(int direction) {
  if (model->scaleY >= -99 || direction > 0) {
    model->scaleY += direction;
    update();
  }
}

void OpenGLViewer::scaleModelZ(int direction) {
  if (model->scaleZ >= -99 || direction > 0) {
    model->scaleZ += direction;
    update();
  }
}

void OpenGLViewer::translateModeleX(int direction) {
  model->translateX += direction;
  update();
}

void OpenGLViewer::translateModeleY(int direction) {
  model->translateY += direction;
  update();
}

void OpenGLViewer::translateModeleZ(int direction) {
  model->translateZ += direction;
  update();
}

void OpenGLViewer::setProj() {
  if (typeProj == 'c')
    typeProj = 'p';
  else if (typeProj == 'p')
    typeProj = 'c';
  update();
}

void OpenGLViewer::saveIMG(int jpg) {
  QOpenGLContext::currentContext()->functions()->glFlush();
  std::vector<char> buffer(this->width() * this->height() * 3);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(0, 0, this->width(), this->height(), GL_RGB, GL_UNSIGNED_BYTE,
               buffer.data());
  stbi_flip_vertically_on_write(true);
  if (jpg) {
    stbi_write_jpg(genFilename(".jpg"), this->width(), this->height(), 3,
                   buffer.data(), 100);
  } else {
    stbi_write_bmp(genFilename(".bmp"), this->width(), this->height(), 3,
                   buffer.data());
  }
}

char *OpenGLViewer::genFilename(QString name) {
  auto filename = QFileDialog::getSaveFileName(
      0, "Save file", QString("%1/render%2").arg(QDir::currentPath()).arg(name),
      QString("%1").arg(name));
  if (filename.isEmpty())
    filename = QString("%1/render%2").arg(QDir::currentPath()).arg(name);
  QByteArray data = filename.toLocal8Bit();
  return data.data();
}

void OpenGLViewer::saveGIF(float x, float y, float z) {
  int width = 640, height = 480, delay = 10, channels = 4, frames = 50;
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadBuffer(GL_BACK_LEFT);
  auto filename = QFileDialog::getSaveFileName(
      0, "Save file", QString("%1/render.gif").arg(QDir::currentPath()),
      ".gif");
  if (filename.isEmpty())
    filename = QString("%1/render.gif").arg(QDir::currentPath());
  QByteArray name = filename.toLocal8Bit();
  GifWriter gif;
  GifBegin(&gif, name.data(), width, height, delay);
  for (int i = 0; i < frames; i++) {
    GifWriteFrame(&gif, writeFrame(width, height, channels).data(), width,
                  height, delay);
    rotateModelX(x / frames);
    rotateModelY(y / frames);
    rotateModelZ(z / frames);
    repaint();
  }
  GifEnd(&gif);
  for (int i = 0; i < frames; i++) {
    rotateModelZ(-z / frames);
    rotateModelY(-y / frames);
    rotateModelX(-x / frames);
  }
  repaint();
}

std::vector<uint8_t> OpenGLViewer::writeFrame(int width, int height,
                                              int channels) {
  std::vector<uint8_t> buffer(width * height * channels);
  glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());

  for (int line = 0; line != height / 2; line++) {
    std::swap_ranges(buffer.begin() + channels * width * line,
                     buffer.begin() + channels * width * (line + 1),
                     buffer.begin() + channels * width * (height - line - 1));
  }
  return buffer;
}
