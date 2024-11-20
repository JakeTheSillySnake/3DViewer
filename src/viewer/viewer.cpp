#include "viewer.h"

#include "ui_viewer.h"

Viewer::Viewer(Controller *controller, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::Viewer) {
  ui->setupUi(this);
  glWindow = new OpenGLViewer(nullptr, controller->m);
  c = controller;
  setCentralWidget(glWindow);
  loadFont();
  viewSetup();
  connectSlots();
  c->changeFile(c->m->path);
  drawField();
}

Viewer::~Viewer() { delete ui; }

void Viewer::loadFont() {
  QFontDatabase::addApplicationFont(
      QFileInfo("../assets/purisa_font.ttf").absoluteFilePath());
  font = new QFont;
  font->setFamily("Purisa");
  font->setBold(true);
  font->setPixelSize(15);

  setFont(*font);
  ui->menubar->setFont(*font);
  ui->menuFile->setFont(*font);
  ui->menuSettings->setFont(*font);
  ui->menuModel->setFont(*font);
  ui->menuProjection_type->setFont(*font);
  ui->menuSave_as->setFont(*font);
  ui->menuSet_texture->setFont(*font);
  ui->menuEdges->setFont(*font);
  ui->menuVertices->setFont(*font);
  ui->menuDisplay_mode->setFont(*font);
}

void Viewer::viewSetup() {
  dock = new QDockWidget;
  addDockWidget(Qt::RightDockWidgetArea, dock);
  dock->setFeatures(dock->features() & ~QDockWidget::DockWidgetClosable);
  view = new QGraphicsView;
  scene = new QGraphicsScene(this);

  view->setScene(scene);
  view->setRenderHint(QPainter::Antialiasing, true);
  view->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  auto content = view->contentsRect();
  view->setSceneRect(0, 0, 200, content.height());
  view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  view->setFrameShape(QFrame::NoFrame);
  view->setDragMode(QGraphicsView::RubberBandDrag);
  view->setStyleSheet("background-color: rgb(36, 31, 49);");

  dock->setWidget(view);
  if (!c->m->showInfo) {
    dock->setVisible(false);
  } else
    ui->actionShow_information->setChecked(true);
  if (c->m->isWireframe) ui->actionWireframe->setChecked(true);
  if (c->m->isDashed) ui->actionLine->setChecked(true);
  if (c->m->pointMode == 0) ui->actionNone->setChecked(true);
  else if (c->m->pointMode == 1) ui->actionSquare->setChecked(true);
  else ui->actionCircle->setChecked(true);
}

void Viewer::connectSlots() {
  connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
  connect(ui->actionUpload_file, SIGNAL(triggered()), this, SLOT(openFile()));
  connect(ui->actionShow_information, SIGNAL(triggered()), this,
          SLOT(showInfo()));
  connect(ui->actionBackground, SIGNAL(triggered()), this,
          SLOT(changeBackground()));
  connect(ui->actionParallel, SIGNAL(triggered()), glWindow, SLOT(setProj()));
  connect(ui->actionCentral, SIGNAL(triggered()), glWindow, SLOT(setProj()));
  connect(ui->actionRotate, SIGNAL(triggered()), this, SLOT(rotate()));
  connect(ui->actionTranslate, SIGNAL(triggered()), this, SLOT(translate()));
  connect(ui->actionScale, SIGNAL(triggered()), this, SLOT(scale()));
  connect(ui->action_jpg, SIGNAL(triggered()), this, SLOT(saveJPG()));
  connect(ui->action_bmp, SIGNAL(triggered()), this, SLOT(saveBMP()));
  connect(ui->actionDefault, SIGNAL(triggered()), this, SLOT(defaultTex()));
  connect(ui->actionWood, SIGNAL(triggered()), this, SLOT(woodTex()));
  connect(ui->actionMetal, SIGNAL(triggered()), this, SLOT(metalTex()));
  connect(ui->actionStone, SIGNAL(triggered()), this, SLOT(stoneTex()));
  connect(ui->actionWater, SIGNAL(triggered()), this, SLOT(waterTex()));
  connect(ui->actionUpload_texture, SIGNAL(triggered()), this, SLOT(openTex()));
  connect(ui->actionMake_screencast, SIGNAL(triggered()), this,
          SLOT(saveGIF()));
  connect(ui->actionWireframe, SIGNAL(triggered()), this,
          SLOT(toggleWireframe()));
  connect(ui->actionColorE, SIGNAL(triggered()), this, SLOT(wireColor()));
  connect(ui->actionThick, SIGNAL(triggered()), this, SLOT(thickness()));
  connect(ui->actionLine, SIGNAL(triggered()), this, SLOT(toggleDashed()));
  connect(ui->actionNone, SIGNAL(triggered()), this, SLOT(pointMode0()));
  connect(ui->actionSquare, SIGNAL(triggered()), this, SLOT(pointMode1()));
  connect(ui->actionCircle, SIGNAL(triggered()), this, SLOT(pointMode2()));
  connect(ui->actionSize, SIGNAL(triggered()), this, SLOT(pointSize()));
  connect(ui->actionColor, SIGNAL(triggered()), this, SLOT(pointColor()));
}

void Viewer::drawField() {
  scene->clear();
  filename = c->m->path;
  edges = (glWindow->vertexes.size() + 1) / 2;
  vertices = glWindow->vertexes.size();
  drawStats();
  view->update();
}

// CONTROLLER STUFF //

void Viewer::pointColor() {
  QColor color = QColorDialog::getColor(Qt::white, this, "Choose Color");
  if (color.isValid()) {
    glWindow->model->pointColor[0] = color.red();
    glWindow->model->pointColor[1] = color.green();
    glWindow->model->pointColor[2] = color.blue();
  }
  glWindow->repaint();
}

void Viewer::pointSize() {
  bool ok;
  int res = QInputDialog::getInt(0, "Point Size", "px", 1, 1, 15, 1, &ok);
  if (ok) glWindow->model->pointSize = res;
  glWindow->repaint();
}

void Viewer::pointMode0() {
  glWindow->model->pointMode = 0;
  ui->actionNone->setChecked(true);
  ui->actionCircle->setChecked(false);
  ui->actionSquare->setChecked(false);
  glWindow->repaint();
}

void Viewer::pointMode1() {
  glWindow->model->pointMode = 1;
  ui->actionNone->setChecked(false);
  ui->actionCircle->setChecked(false);
  ui->actionSquare->setChecked(true);
  glWindow->repaint();
}

void Viewer::pointMode2() {
  glWindow->model->pointMode = 2;
  ui->actionNone->setChecked(false);
  ui->actionCircle->setChecked(true);
  ui->actionSquare->setChecked(false);
  glWindow->repaint();
}

void Viewer::toggleDashed() {
  if (ui->actionLine->isChecked())
    glWindow->model->isDashed = true;
  else
    glWindow->model->isDashed = false;
  glWindow->repaint();
}

void Viewer::thickness() {
  bool ok;
  int res = QInputDialog::getInt(0, "Line Width", "px", 1, 1, 20, 1, &ok);
  if (ok) glWindow->model->lineSize = res;
  glWindow->repaint();
}

void Viewer::wireColor() {
  QColor color = QColorDialog::getColor(Qt::white, this, "Choose Color");
  if (color.isValid()) {
    glWindow->model->wireColor[0] = color.red();
    glWindow->model->wireColor[1] = color.green();
    glWindow->model->wireColor[2] = color.blue();
  }
  glWindow->repaint();
}

void Viewer::toggleWireframe() {
  if (ui->actionWireframe->isChecked())
    glWindow->model->isWireframe = true;
  else
    glWindow->model->isWireframe = false;
  glWindow->repaint();
}

void Viewer::saveGIF() {
  std::pair<QStringList, bool> res = inputBox("Rotate Animation");
  if (res.second) {
    auto it = res.first.begin();
    glWindow->saveGIF((*it++).toFloat(), (*it++).toFloat(), (*it).toFloat());
  }
}

void Viewer::openTex() {
  filename =
      QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath(),
                                   tr("Images (*jpg *jpeg *bmp *png *tiff)"));
  if (!filename.isEmpty()) glWindow->setTexture(filename);
}

void Viewer::defaultTex() {
  glWindow->setTexture("../assets/textures/default.png");
}

void Viewer::woodTex() { glWindow->setTexture("../assets/textures/wood.png"); }

void Viewer::metalTex() {
  glWindow->setTexture("../assets/textures/metal.png");
}

void Viewer::stoneTex() {
  glWindow->setTexture("../assets/textures/stone.png");
}

void Viewer::waterTex() {
  glWindow->setTexture("../assets/textures/water.png");
}

void Viewer::saveJPG() { glWindow->saveIMG(1); }

void Viewer::saveBMP() { glWindow->saveIMG(0); }

void Viewer::openFile() {
  filename = QFileDialog::getOpenFileName(
      this, tr("Open File"), QDir::currentPath(), tr("Object Files (*.obj)"));
  if (!filename.isEmpty()) c->changeFile(filename);
  drawField();
}

void Viewer::showInfo() {
  if (ui->actionShow_information->isChecked())
    dock->setVisible(true);
  else
    dock->setVisible(false);
  c->m->showInfo = dock->isVisible();
}

void Viewer::rotate() {
  std::pair<QStringList, bool> res = inputBox("Rotate Model");
  if (res.second) {
    auto it = res.first.begin();
    glWindow->rotateModelX((*it++).toFloat());
    glWindow->rotateModelY((*it++).toFloat());
    glWindow->rotateModelZ((*it).toFloat());
  }
}

void Viewer::translate() {
  std::pair<QStringList, bool> res = inputBox("Translate Model");
  if (res.second) {
    auto it = res.first.begin();
    glWindow->translateModeleX((*it++).toFloat());
    glWindow->translateModeleY((*it++).toFloat());
    glWindow->translateModeleZ((*it).toFloat());
  }
}

void Viewer::scale() {
  std::pair<QStringList, bool> res = inputBox("Scale Model");
  if (res.second) {
    auto it = res.first.begin();
    glWindow->scaleModelX((*it++).toFloat());
    glWindow->scaleModelY((*it++).toFloat());
    glWindow->scaleModelZ((*it).toFloat());
  }
}

void Viewer::changeBackground() {
  QColor color = QColorDialog::getColor(Qt::white, this, "Choose Color");
  if (color.isValid())
    glWindow->setVec3(color.red(), color.green(), color.blue());
  glWindow->repaint();
  glWindow->update();
}

// END OF CONTROLLER STUFF //

std::pair<QStringList, bool> Viewer::inputBox(QString action) {
  std::pair<QStringList, bool> res;
  QDialog dialog(this);
  dialog.setWindowTitle(action);
  dialog.setParent(0);
  QFormLayout form(&dialog);
  form.addRow(new QLabel("Degrees:"));
  QLineEdit *fields[3];
  QString field_names[3] = {"X", "Y", "Z"};

  for (int i = 0; i < 3; i++) {
    QLineEdit *line = new QLineEdit(&dialog);
    QString label = QString("%1-axis").arg(field_names[i]);
    form.addRow(label, line);
    fields[i] = line;
  }
  QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                           Qt::Horizontal, &dialog);
  form.addRow(&buttons);
  connect(&buttons, SIGNAL(accepted()), &dialog, SLOT(accept()));
  connect(&buttons, SIGNAL(rejected()), &dialog, SLOT(reject()));
  QStringList vals;
  if (dialog.exec() == QDialog::Accepted) {
    for (int i = 0; i < 3; i++) vals.push_back(fields[i]->text());
    res.second = true;
  } else
    res.second = false;
  res.first = vals;
  return res;
}

void Viewer::drawStats() {
  QStringList pieces = filename.split("/");
  QString formattedName;
  if (pieces.length() > 5) {
    QList<QString>::iterator it = ++pieces.begin();
    formattedName = QString("/%1/%2/%3/.../%4")
                        .arg(*(it++))
                        .arg(*(it++))
                        .arg(*(it++))
                        .arg(*(--pieces.end()));
  } else
    formattedName = filename;
  auto text = scene->addSimpleText(QString("File: %1").arg(formattedName));
  addText(*font, text, 5, 3);
  text = scene->addSimpleText(QString("Edges: %1").arg(QString::number(edges)));
  addText(*font, text, 5, 35);
  text = scene->addSimpleText(
      QString("Vertices: %1").arg(QString::number(vertices)));
  addText(*font, text, 5, 55);
  text = scene->addSimpleText(
      QString("\nControls:\n\n\nRotate:\n\n\nTranslate:\n\n\nScale:"));
  addText(*font, text, 5, 75);
  drawControls();
}

void Viewer::drawControls() {
  int x = 7, y = 125, size = 20, space = 3;
  QString buttons[6] = {"Q", "W", "E", "A", "S", "D"};
  for (int i = 0; i < 6; i++) {
    addButton(x, y, size, size, buttons[i % 6]);
    x += size + space;
    if (x > (size * 3)) {
      y += size + space;
      x = 7;
    }
  }
  auto text = scene->addSimpleText("+");
  addText(*font, text, 50, 275);
  text = scene->addSimpleText("+");
  addText(*font, text, 60, 350);
  addButton(7, 200, size * 4 - 5, size + 5, "Controls");
  addButton(7, 275, size * 2, size + 5, "Alt");
  addButton(65, 275, size * 4 - 5, size + 5, "Controls");
  addButton(7, 350, size * 2 + 10, size + 5, "Shift");
  addButton(75, 350, size * 4 - 5, size + 5, "Controls");
}

void Viewer::addButton(int x, int y, int width, int height, QString text) {
  QPushButton *button = new QPushButton;
  button->setGeometry(QRect(x, y, width, height));
  button->setText(text);
  button->setDisabled(true);
  button->setStyleSheet(
      "background-color: transparent; color: rgb(43, 183, 233); font-family: "
      "'Purisa'; "
      "border: 1px solid rgb(43, 183, 233); border-radius: 5px;");
  scene->addWidget(button);
}

void Viewer::addText(QFont font, QGraphicsSimpleTextItem *text, int x, int y) {
  text->setPos(x, y);
  text->setFont(font);
  text->setBrush(Qt::white);
}

void Viewer::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
    case Qt::Key_S:
      if (event->modifiers() == Qt::ShiftModifier)
        emit glWindow->scaleModelSignalY(-1);
      else if (event->modifiers() == Qt::AltModifier)
        emit glWindow->translateModeleSignalY(-1);
      else
        emit glWindow->rotateModelSignalX(1.0);
      break;
    case Qt::Key_W:
      if (event->modifiers() == Qt::ShiftModifier)
        emit glWindow->scaleModelSignalY(1);
      else if (event->modifiers() == Qt::AltModifier)
        emit glWindow->translateModeleSignalY(1);
      else
        emit glWindow->rotateModelSignalX(-1.0);
      break;
    case Qt::Key_Q:
      if (event->modifiers() == Qt::ShiftModifier)
        emit glWindow->scaleModelSignalX(1);
      else if (event->modifiers() == Qt::AltModifier)
        emit glWindow->translateModeleSignalZ(-1);
      else
        emit glWindow->rotateModelSignalY(-1.0);
      break;
    case Qt::Key_E:
      if (event->modifiers() == Qt::ShiftModifier)
        emit glWindow->scaleModelSignalX(-1);
      else if (event->modifiers() == Qt::AltModifier)
        emit glWindow->translateModeleSignalZ(1);
      else
        emit glWindow->rotateModelSignalY(1.0);
      break;
    case Qt::Key_A:
      if (event->modifiers() == Qt::ShiftModifier)
        emit glWindow->scaleModelSignalZ(1);
      else if (event->modifiers() == Qt::AltModifier)
        emit glWindow->translateModeleSignalX(-1);
      else
        emit glWindow->rotateModelSignalZ(1.0);
      break;
    case Qt::Key_D:
      if (event->modifiers() == Qt::ShiftModifier)
        emit glWindow->scaleModelSignalZ(-1);
      else if (event->modifiers() == Qt::AltModifier)
        emit glWindow->translateModeleSignalX(1);
      else
        emit glWindow->rotateModelSignalZ(-1.0);
      break;
  }
  drawField();
}
