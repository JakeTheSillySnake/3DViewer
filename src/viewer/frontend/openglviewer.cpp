#include "openglviewer.h"

#include "../../external/gif.h"  // saving gif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#define STB_IMAGE_WRITE_IMPLEMENTATION  // saving jpg, bmp
#include "../../external/stb_image_write.h"
#pragma GCC diagnostic pop

using namespace s21;

OpenGLViewer::OpenGLViewer(QWidget *parent, Controller *c)
    : QOpenGLWidget{parent},
      m_texture(0),
      m_indexBuffer(QOpenGLBuffer::IndexBuffer),
      c(c) {}

void OpenGLViewer::initializeGL() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  initShaders();
  dataProcessing();
  connect(c->model, SIGNAL(updateSignal()), this, SLOT(update()));
  connect(c->model, SIGNAL(uploadFileSignal()), this, SLOT(dataProcessing()));
  connect(c->model, SIGNAL(repaintModelSignal()), this, SLOT(repaint()));
}

void OpenGLViewer::resizeGL(int nWidth, int nHeight) {
  float aspect = nWidth / (float)nHeight;
  projectMatrix.setToIdentity();
  if (!c->model->isParallel)
    projectMatrix.perspective(45, aspect, 0.1f, 800.0f);
  else
    projectMatrix.ortho(-4.0f, 4.0f, -4.0f, 4.0f, 0.1f, 800.0f);
}

void OpenGLViewer::paintGL() {
  resizeGL(this->width(), this->height());
  setMode();
  setMatrix();
  if (c->model->isWireframe && c->model->pointMode) {
    genTexture(c->model->pointColor[0], c->model->pointColor[1],
               c->model->pointColor[2]);
    glDrawElements(GL_TRIANGLES, m_indexBuffer.size(), GL_UNSIGNED_INT, 0);
  }
  if (c->model->isWireframe) {
    genTexture(c->model->wireColor[0], c->model->wireColor[1],
               c->model->wireColor[2]);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  glDrawElements(GL_TRIANGLES, m_indexBuffer.size(), GL_UNSIGNED_INT, 0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void OpenGLViewer::setMode() {
  setTexture(c->model->texPath);
  if (c->model->isDashed && c->model->isWireframe) {
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(4, 0xAAAA);
  } else {
    glDisable(GL_LINE_STIPPLE);
  }
  if (c->model->isWireframe && c->model->pointMode) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    if (c->model->pointMode == 2) {
      glEnable(GL_POINT_SMOOTH);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
      glDisable(GL_POINT_SMOOTH);
      glDisable(GL_BLEND);
    }
  }
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(c->model->bgColor[0] / 255.0, c->model->bgColor[1] / 255.0,
               c->model->bgColor[2] / 255.0, 1.0f);
  glLineWidth(c->model->lineSize);
  glPointSize(c->model->pointSize);
}

void OpenGLViewer::setMatrix() {
  modelViewMatrix.setToIdentity();
  modelViewMatrix.translate(0.01 * c->model->translateX,
                            0.01 * c->model->translateY,
                            0.01 * c->model->translateZ - 5.0);
  modelViewMatrix.rotate(c->model->rotation);
  modelViewMatrix.rotate(c->model->rotationX, 1, 0, 0);
  modelViewMatrix.rotate(c->model->rotationY, 0, 1, 0);
  QMatrix4x4 scalingMatrix;
  scalingMatrix.scale(1.0f + 0.01 * c->model->scaleX,
                      1.0f + 0.01 * c->model->scaleY,
                      1.0f + 0.01 * c->model->scaleZ);
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
  m_arrayBuffer.allocate(c->model->triangles.constData(),
                         c->model->triangles.size() * sizeof(VertexData));
  m_indexBuffer.create();
  m_indexBuffer.bind();
  m_indexBuffer.allocate(c->model->indices.constData(),
                         c->model->indices.size() * sizeof(GLuint));
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
  QFile tex = QFileInfo(c->model->texPath).absoluteFilePath();
  if (!tex.exists()) c->model->texPath = "../../assets/textures/default.png";
  m_texture = new QOpenGLTexture(
      QImage(QFileInfo(c->model->texPath).absoluteFilePath()).mirrored());
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

void OpenGLViewer::mousePressEvent(QMouseEvent *event) {
  mousePos = event->pos();
  c->model->prevRotX = c->model->rotationX;
  c->model->prevRotY = c->model->rotationY;
}

void OpenGLViewer::mouseMoveEvent(QMouseEvent *event) {
  c->model->rotationY = c->model->prevRotY + event->pos().x() - mousePos.x();
  c->model->rotationX = c->model->prevRotX + event->pos().y() - mousePos.y();
  update();
}
void OpenGLViewer::mouseReleaseEvent(QMouseEvent *event) {
  c->model->prevRotX = c->model->rotationX;
  c->model->prevRotY = c->model->rotationY;
  mousePos = event->pos();
}

void OpenGLViewer::wheelEvent(QWheelEvent *event) {
  QPoint pixels = event->pixelDelta(), angle = event->angleDelta() / 8;
  int steps = 0;
  if (!pixels.isNull())
    steps = pixels.y();
  else if (!angle.isNull())
    steps = angle.y() / 15;
  c->scaleModel(steps, steps, steps);
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
    c->rotateModel(x / frames, y / frames, z / frames);
    repaint();
  }
  GifEnd(&gif);
  for (int i = 0; i < frames; i++) {
    c->rotateModel(0, 0, -z / frames);
    c->rotateModel(0, -y / frames, 0);
    c->rotateModel(-x / frames, 0, 0);
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
