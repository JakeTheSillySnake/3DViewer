#include "openglviewer.h"

#include "../../external/gif.h"  // saving

#define STB_IMAGE_WRITE_IMPLEMENTATION  // saving jpg, bmp
#include "../../external/stb_image_write.h"

using namespace s21;

OpenGLViewer::OpenGLViewer(QWidget *parent, Model *model)
    : QOpenGLWidget{parent},
      m_texture(0),
      m_indexBuffer(QOpenGLBuffer::IndexBuffer),
      model(model) {}

void OpenGLViewer::initializeGL() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  initShaders();
  dataProcessing();
  connect(model, SIGNAL(updateSignal()), this, SLOT(update()));
  connect(model, SIGNAL(uploadFileSignal()), this, SLOT(dataProcessing()));
  connect(model, SIGNAL(repaintModelSignal()), this, SLOT(repaint()));
}

void OpenGLViewer::resizeGL(int nWidth, int nHeight) {
  float aspect = nWidth / (float)nHeight;
  projectMatrix.setToIdentity();
  if (!model->isParallel)
    projectMatrix.perspective(45, aspect, 0.1f, 800.0f);
  else
    projectMatrix.ortho(-4.0f, 4.0f, -4.0f, 4.0f, 0.1f, 800.0f);
}

void OpenGLViewer::paintGL() {
  resizeGL(this->width(), this->height());
  setMode();
  setMatrix();
  if (model->isWireframe && model->pointMode) {
    genTexture(model->pointColor[0], model->pointColor[1],
               model->pointColor[2]);
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
  setTexture(model->texPath);
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
  glLineWidth(model->lineSize);
  glPointSize(model->pointSize);
}

void OpenGLViewer::setMatrix() {
  modelViewMatrix.setToIdentity();
  modelViewMatrix.translate(0.01 * model->translateX, 0.01 * model->translateY,
                            0.01 * model->translateZ - 5.0);
  modelViewMatrix.rotate(model->rotation);
  modelViewMatrix.rotate(model->rotationX, 1, 0, 0);
  modelViewMatrix.rotate(model->rotationY, 0, 1, 0);
  QMatrix4x4 scalingMatrix;
  scalingMatrix.scale(1.0f + 0.01 * model->scaleX, 1.0f + 0.01 * model->scaleY,
                      1.0f + 0.01 * model->scaleZ);
  modelViewMatrix *= scalingMatrix;
  m_program.setUniformValue("qt_ModelViewProjectionMatrix",
                            projectMatrix * modelViewMatrix);
  m_arrayBuffer.bind();
  int vertLoc = m_program.attributeLocation("qt_Vertex");
  m_program.enableAttributeArray(vertLoc);
  m_program.setAttributeBuffer(vertLoc, GL_FLOAT, 0, 3, sizeof(VertexData));
  m_indexBuffer.bind();
  int textLoc = m_program.attributeLocation("qt_MultiTexCoord0");
  m_program.enableAttributeArray(textLoc);
  m_program.setAttributeBuffer(textLoc, GL_FLOAT, sizeof(QVector3D), 2,
                               sizeof(VertexData));
  m_program.setUniformValue("qt_Texture0", 0);
}

void OpenGLViewer::dataProcessing() {
  if (m_arrayBuffer.isCreated()) m_arrayBuffer.destroy();
  if (m_indexBuffer.isCreated()) m_indexBuffer.destroy();
  m_arrayBuffer.create();
  m_arrayBuffer.bind();
  m_arrayBuffer.allocate(model->triangles.constData(),
                         model->triangles.size() * sizeof(VertexData));
  m_indexBuffer.create();
  m_indexBuffer.bind();
  m_indexBuffer.allocate(model->indices.constData(),
                         model->indices.size() * sizeof(GLuint));
  repaint();
}

void OpenGLViewer::genTexture(float red, float green, float blue) {
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  uint8_t *data = new uint8_t[3];
  data[0] = (uint8_t)red;
  data[1] = (uint8_t)green;
  data[2] = (uint8_t)blue;
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE,
               data);
  delete[] data;
}

void OpenGLViewer::setTexture(QString filename) {
  if (m_texture) delete m_texture;
  m_texture = new QOpenGLTexture(
      QImage(QFileInfo(filename).absoluteFilePath()).mirrored());
  m_texture->bind(0);
}

void OpenGLViewer::initShaders() {
  QFile tex = QFileInfo(model->texPath).absoluteFilePath();
  if (!tex.exists()) model->texPath = "../../assets/textures/default.png";
  m_texture = new QOpenGLTexture(
      QImage(QFileInfo(model->texPath).absoluteFilePath()).mirrored());
  m_program.addShaderFromSourceFile(
      QOpenGLShader::Vertex,
      QFileInfo("../../assets/shaders/vshader.vsh").absoluteFilePath());
  m_program.addShaderFromSourceFile(
      QOpenGLShader::Fragment,
      QFileInfo("../../assets/shaders/fshader.fsh").absoluteFilePath());
  m_program.link();
  m_program.bind();
}

OpenGLViewer::~OpenGLViewer() {
  delete m_texture;
  m_indexBuffer.destroy();
  m_arrayBuffer.destroy();
}

void OpenGLViewer::rotateModel(float angleX, float angleY, float angleZ) {
  QVector3D axis[3] = {QVector3D(1.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0),
                       QVector3D(0.0, 0.0, 1.0)};
  float angle[3] = {angleX, angleY, angleZ};
  for (int i = 0; i < 3; i++) {
    model->rotation *= QQuaternion::fromAxisAndAngle(axis[i], angle[i]);
    update();
  }
}

void OpenGLViewer::scaleModel(float dirX, float dirY, float dirZ) {
  if (model->scaleX >= -99 || dirX > 0) model->scaleX += dirX;
  if (model->scaleY >= -99 || dirY > 0) model->scaleY += dirY;
  if (model->scaleZ >= -99 || dirZ > 0) model->scaleZ += dirZ;
  update();
}

void OpenGLViewer::translateModel(float dirX, float dirY, float dirZ) {
  model->translateX += dirX;
  model->translateY += dirY;
  model->translateZ += dirZ;
  update();
}

void OpenGLViewer::mousePressEvent(QMouseEvent *event) {
  mousePos = event->pos();
  model->prevRotX = model->rotationX;
  model->prevRotY = model->rotationY;
}

void OpenGLViewer::mouseMoveEvent(QMouseEvent *event) {
  model->rotationY = model->prevRotY + event->pos().x() - mousePos.x();
  model->rotationX = model->prevRotX + event->pos().y() - mousePos.y();
  update();
}
void OpenGLViewer::mouseReleaseEvent(QMouseEvent *event) {
  model->prevRotX = model->rotationX;
  model->prevRotY = model->rotationY;
  mousePos = event->pos();
}

void OpenGLViewer::mouseWheelEvent(QWheelEvent *event) {
  QPoint pixels = event->pixelDelta(), angle = event->angleDelta() / 8;
  int steps = 0;
  if (!pixels.isNull()) steps = pixels.y();
  else if (!angle.isNull()) steps = angle.y() / 15;
  double scaleFactor = 1.0 + steps * 0.001f;
  if (scaleFactor < 0.1f) scaleFactor = 0.1f;
  scaleModel(scaleFactor, scaleFactor, scaleFactor);
}

void OpenGLViewer::saveIMG(int jpg) {
  QOpenGLContext::currentContext()->functions()->glFlush();
  std::vector<char> buffer(this->width() * this->height() * 3);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(0, 12, this->width(), this->height(), GL_RGB, GL_UNSIGNED_BYTE,
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
    rotateModel(x / frames, y / frames, z / frames);
    repaint();
  }
  GifEnd(&gif);
  for (int i = 0; i < frames; i++) {
    rotateModel(0, 0, -z / frames);
    rotateModel(0, -y / frames, 0);
    rotateModel(-x / frames, 0, 0);
  }
  repaint();
}

std::vector<uint8_t> OpenGLViewer::writeFrame(int width, int height,
                                              int channels) {
  std::vector<uint8_t> buffer(width * height * channels);
  glReadPixels(0, 12, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());
  for (int line = 0; line != height / 2; line++) {
    std::swap_ranges(buffer.begin() + channels * width * line,
                     buffer.begin() + channels * width * (line + 1),
                     buffer.begin() + channels * width * (height - line - 1));
  }
  return buffer;
}
