#include "viewer.h"

#include "ui_viewer.h"

using namespace s21;

Viewer::Viewer(Controller *controller, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::Viewer) {
  ui->setupUi(this);
  glWindow = new OpenGLViewer(nullptr, controller->model);
  c = controller;
  setCentralWidget(glWindow);
  loadFont();
  viewSetup();
  connectSlots();
  c->changeFile(c->model->path);
  drawField();
}

Viewer::~Viewer() { delete ui; }

void Viewer::loadFont() {
  QFontDatabase::addApplicationFont(
      QFileInfo("../../assets/purisa_font.ttf").absoluteFilePath());
  font = new QFont;
  font->setFamily("Purisa");
  font->setBold(true);
  font->setPixelSize(15);
  setFont(*font);
  std::vector<QMenu *> items = {ui->menuFile,        ui->menuSettings,
                                ui->menuSave_as,     ui->menuSet_texture,
                                ui->menuEdges,       ui->menuVertices,
                                ui->menuDisplay_mode, ui->menuModel};
  for (int i = 0; i < (int)items.size(); i++) items[i]->setFont(*font);
  ui->menubar->setFont(*font);
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
  if (!c->model->showInfo) {
    dock->setVisible(false);
  } else
    ui->actionShow_information->setChecked(true);
  if (c->model->isWireframe) ui->actionWireframe->setChecked(true);
  if (c->model->isDashed) ui->actionLine->setChecked(true);
  if (c->model->pointMode == 0)
    ui->actionNone->setChecked(true);
  else if (c->model->pointMode == 1)
    ui->actionSquare->setChecked(true);
  else
    ui->actionCircle->setChecked(true);
  if (c->model->isParallel) ui->actionParallel->setChecked(true);
}

void Viewer::connectSlots() {
  connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
  connect(ui->actionUpload_file, SIGNAL(triggered()), this, SLOT(openFile()));
  connect(ui->actionShow_information, SIGNAL(triggered()), this,
          SLOT(showInfo()));
  connect(ui->actionBackground, SIGNAL(triggered()), c,
          SLOT(changeBackground()));
  connect(ui->actionParallel, SIGNAL(triggered()), this, SLOT(toggleProj()));
  connect(ui->actionRotate, SIGNAL(triggered()), this, SLOT(rotate()));
  connect(ui->actionTranslate, SIGNAL(triggered()), this, SLOT(translate()));
  connect(ui->actionScale, SIGNAL(triggered()), this, SLOT(scale()));
  connect(ui->action_jpg, SIGNAL(triggered()), this, SLOT(saveJPG()));
  connect(ui->action_bmp, SIGNAL(triggered()), this, SLOT(saveBMP()));
  connect(ui->actionDefault, SIGNAL(triggered()), c, SLOT(defaultTex()));
  connect(ui->actionWood, SIGNAL(triggered()), c, SLOT(woodTex()));
  connect(ui->actionMetal, SIGNAL(triggered()), c, SLOT(metalTex()));
  connect(ui->actionStone, SIGNAL(triggered()), c, SLOT(stoneTex()));
  connect(ui->actionWater, SIGNAL(triggered()), c, SLOT(waterTex()));
  connect(ui->actionUpload_texture, SIGNAL(triggered()), c, SLOT(openTex()));
  connect(ui->actionMake_screencast, SIGNAL(triggered()), this,
          SLOT(saveGIF()));
  connect(ui->actionWireframe, SIGNAL(triggered()), this,
          SLOT(toggleWireframe()));
  connect(ui->actionColorE, SIGNAL(triggered()), c, SLOT(wireColor()));
  connect(ui->actionThick, SIGNAL(triggered()), c, SLOT(thickness()));
  connect(ui->actionLine, SIGNAL(triggered()), this, SLOT(toggleDashed()));
  connect(ui->actionNone, SIGNAL(triggered()), this, SLOT(pointMode0()));
  connect(ui->actionSquare, SIGNAL(triggered()), this, SLOT(pointMode1()));
  connect(ui->actionCircle, SIGNAL(triggered()), this, SLOT(pointMode2()));
  connect(ui->actionSize, SIGNAL(triggered()), c, SLOT(pointSize()));
  connect(ui->actionColor, SIGNAL(triggered()), c, SLOT(pointColor()));
}

void Viewer::drawField() {
  scene->clear();
  edges = (c->model->triangles.size() + 1) / 2;
  vertices = c->model->triangles.size();
  if (edges) filename = c->model->path;
  else filename = "";
  drawStats();
  view->update();
}

void Viewer::pointMode0() {
  c->pointMode0();
  ui->actionNone->setChecked(true);
  ui->actionCircle->setChecked(false);
  ui->actionSquare->setChecked(false);
}

void Viewer::pointMode1() {
  c->pointMode1();
  ui->actionNone->setChecked(false);
  ui->actionCircle->setChecked(false);
  ui->actionSquare->setChecked(true);
}

void Viewer::pointMode2() {
  c->pointMode2();
  ui->actionNone->setChecked(false);
  ui->actionCircle->setChecked(true);
  ui->actionSquare->setChecked(false);
}

void Viewer::toggleDashed() {
  glWindow->model->isDashed = ui->actionLine->isChecked();
  glWindow->repaint();
}

void Viewer::toggleWireframe() {
  glWindow->model->isWireframe = ui->actionWireframe->isChecked();
  glWindow->repaint();
}

void Viewer::toggleProj() {
  glWindow->model->isParallel = ui->actionParallel->isChecked();
  glWindow->repaint();
}

void Viewer::saveGIF() {
  std::pair<QStringList, bool> res = inputBox("Rotate Animation");
  if (res.second) {
    auto it = res.first.begin();
    float x = (*it++).toFloat(), y = (*it++).toFloat(), z = (*it).toFloat();
    glWindow->saveGIF(x, y, z);
  }
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
  dock->setVisible(ui->actionShow_information->isChecked());
  c->model->showInfo = dock->isVisible();
}

void Viewer::rotate() {
  std::pair<QStringList, bool> res = inputBox("Rotate Model");
  if (res.second) {
    auto it = res.first.begin();
    float x = (*it++).toFloat(), y = (*it++).toFloat(), z = (*it).toFloat();
    glWindow->rotateModel(x, y, z);
  }
}

void Viewer::translate() {
  std::pair<QStringList, bool> res = inputBox("Translate Model");
  if (res.second) {
    auto it = res.first.begin();
    float x = (*it++).toFloat(), y = (*it++).toFloat(), z = (*it).toFloat();
    glWindow->translateModel(x, y, z);
  }
}

void Viewer::scale() {
  std::pair<QStringList, bool> res = inputBox("Scale Model");
  if (res.second) {
    auto it = res.first.begin();
    float x = (*it++).toFloat(), y = (*it++).toFloat(), z = (*it).toFloat();
    glWindow->scaleModel(x, y, z);
  }
}

std::pair<QStringList, bool> Viewer::inputBox(QString action) {
  std::pair<QStringList, bool> res;
  QDialog dialog(0);
  dialog.setWindowTitle(action);
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
  QString formattedName = *(--pieces.end());
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
        glWindow->scaleModel(0, -1, 0);
      else if (event->modifiers() == Qt::AltModifier)
        glWindow->translateModel(0, -1, 0);
      else
        glWindow->rotateModel(1, 0, 0);
      break;
    case Qt::Key_W:
      if (event->modifiers() == Qt::ShiftModifier)
        glWindow->scaleModel(0, 1, 0);
      else if (event->modifiers() == Qt::AltModifier)
        glWindow->translateModel(0, 1, 0);
      else
        glWindow->rotateModel(-1, 0, 0);
      break;
    case Qt::Key_Q:
      if (event->modifiers() == Qt::ShiftModifier)
        glWindow->scaleModel(1, 0, 0);
      else if (event->modifiers() == Qt::AltModifier)
        glWindow->translateModel(0, 0, -1);
      else
        glWindow->rotateModel(0, -1, 0);
      break;
    case Qt::Key_E:
      if (event->modifiers() == Qt::ShiftModifier)
        glWindow->scaleModel(-1, 0, 0);
      else if (event->modifiers() == Qt::AltModifier)
        glWindow->translateModel(0, 0, 1);
      else
        glWindow->rotateModel(0, 1, 0);
      break;
    case Qt::Key_A:
      if (event->modifiers() == Qt::ShiftModifier)
        glWindow->scaleModel(0, 0, 1);
      else if (event->modifiers() == Qt::AltModifier)
        glWindow->translateModel(-1, 0, 0);
      else
        glWindow->rotateModel(0, 0, 1);
      break;
    case Qt::Key_D:
      if (event->modifiers() == Qt::ShiftModifier)
        glWindow->scaleModel(0, 0, -1);
      else if (event->modifiers() == Qt::AltModifier)
        glWindow->translateModel(1, 0, 0);
      else
        glWindow->rotateModel(0, 0, -1);
      break;
  }
  drawField();
}
