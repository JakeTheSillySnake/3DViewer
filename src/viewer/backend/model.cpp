#include "model.h"

void Model::loadOBJ() {
  fileLines.clear();
  QFile file(QFileInfo(path).absoluteFilePath());
  if (!file.exists()) {
    return;
  }
  file.open(QIODevice::ReadOnly);
  QTextStream input(&file);
  while (!input.atEnd()) {
    QString str = input.readLine();
    fileLines.append(str);
  }
  file.close();
}

void Model::changeModelSendSignal() { emit changeFileSignal(); }

void Model::loadSettings() {
  QFile file(QFileInfo(settPath).absoluteFilePath());
  if (!file.exists()) return;
  file.open(QIODevice::ReadOnly);
  QTextStream input(&file);
  while (!input.atEnd()) {
    QString str = input.readLine();
    QStringList setting = str.split("=");
    if (setting[0] == "bg_red")
      bgColor[0] = setting[1].toInt();
    else if (setting[0] == "bg_green")
      bgColor[1] = setting[1].toInt();
    else if (setting[0] == "bg_blue")
      bgColor[2] = setting[1].toInt();
    else if (setting[0] == "wire_red")
      wireColor[0] = setting[1].toInt();
    else if (setting[0] == "wire_green")
      wireColor[1] = setting[1].toInt();
    else if (setting[0] == "wire_blue")
      wireColor[2] = setting[1].toInt();
    else if (setting[0] == "isWire") {
      if (setting[1].toInt()) isWireframe = true;
    } else if (setting[0] == "isDashed") {
      if (setting[1].toInt()) isDashed = true;
    } else if (setting[0] == "wireWidth")
      lineSize = setting[1].toInt();
    else if (setting[0] == "showInfo") {
      if (!setting[1].toInt()) showInfo = false;
    } else if (setting[0] == "filePath")
      path = setting[1];
    else if (setting[0] == "texPath")
      texPath = setting[1];
    else if (setting[0] == "pointMode")
      pointMode = setting[1].toInt();
    else if (setting[0] == "pointSize")
      pointSize = setting[1].toInt();
    else if (setting[0] == "point_red")
      pointColor[0] = setting[1].toInt();
    else if (setting[0] == "point_green")
      pointColor[1] = setting[1].toInt();
    else if (setting[0] == "point_blue")
      pointColor[2] = setting[1].toInt();
  }
  file.close();
}

void Model::saveSettings() {
  FILE *fptr = fopen(settPath, "w");
  auto filePath = path.toLocal8Bit(), texturePath = texPath.toLocal8Bit();
  fprintf(fptr,
          "bg_red=%d\nbg_green=%d\nbg_blue=%d\nwire_red=%d\nwire_green=%"
          "d\nwire_blue=%d\npointMode=%d\npointSize=%d\nwireWidth=%d\nisWire=%"
          "d\nisDashed=%d\nshowInfo=%d\nfilePath=%s\ntexPath=%s\npoint_red=%d\npoint_green=%d\npoint_blue=%d",
          (int)bgColor[0], (int)bgColor[1], (int)bgColor[2], (int)wireColor[0],
          (int)wireColor[1], (int)wireColor[2], pointMode, pointSize, lineSize,
          isWireframe, isDashed, showInfo, filePath.data(), texturePath.data(), (int)pointColor[0], (int)pointColor[1], (int)pointColor[2]);
  fclose(fptr);
}
