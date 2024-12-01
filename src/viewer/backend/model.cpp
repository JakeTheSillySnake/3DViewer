#include "model.h"

#define TINYOBJLOADER_IMPLEMENTATION  // loading models
#include "../../external/tiny_obj_loader.h"

using namespace s21;

void Model::loadOBJ() {
  QFile file(QFileInfo(path).absoluteFilePath());
  if (!file.exists()) {
    return;
  }
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;
  auto pathData = path.toLocal8Bit();
  bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                                  pathData.data(), "../../assets/models");
  if (!err.empty()) std::cerr << err << std::endl;
  if (!success)
    std::cerr << "Oops! Unable to load file " << pathData.data() << std::endl;
  else {
    QVector<QVector3D> vertices, normals;
    QVector<QVector2D> uvs;
    triangles.clear();
    indices.clear();
    for (size_t i = 0; i < shapes.size(); i++) {
      tinyobj::shape_t &shape = shapes[i];
      tinyobj::mesh_t &mesh = shape.mesh;
      for (size_t j = 0; j < mesh.indices.size(); j++) {
        tinyobj::index_t index = mesh.indices[j];
        QVector3D pos = {attrib.vertices[index.vertex_index * 3],
                         attrib.vertices[index.vertex_index * 3 + 1],
                         attrib.vertices[index.vertex_index * 3 + 2]};
        QVector3D normal = {attrib.normals[index.normal_index * 3],
                            attrib.normals[index.normal_index * 3 + 1],
                            attrib.normals[index.normal_index * 3 + 2]};
        QVector2D texCoord = {attrib.texcoords[index.texcoord_index * 2],
                              attrib.texcoords[index.texcoord_index * 2 + 1]};
        triangles.push_back(VertexData(pos, texCoord, normal));
        indices.push_back(indices.size());
      }
    }
  }
}

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
    else if (setting[0] == "isParallel")
      if (setting[1].toInt()) isParallel = true;
  }
  file.close();
}

void Model::saveSettings() {
  auto filePath = path.toLocal8Bit(), texturePath = texPath.toLocal8Bit(),
       settings = settPath.toLocal8Bit();
  FILE *fptr = fopen(settings.data(), "w");
  fprintf(fptr,
          "bg_red=%d\nbg_green=%d\nbg_blue=%d\nwire_red=%d\nwire_green=%"
          "d\nwire_blue=%d\npointMode=%d\npointSize=%d\nwireWidth=%d\nisWire=%"
          "d\nisDashed=%d\nshowInfo=%d\nfilePath=%s\ntexPath=%s\npoint_red=%"
          "d\npoint_green=%d\npoint_blue=%d\nisParallel=%d",
          (int)bgColor[0], (int)bgColor[1], (int)bgColor[2], (int)wireColor[0],
          (int)wireColor[1], (int)wireColor[2], pointMode, pointSize, lineSize,
          isWireframe, isDashed, showInfo, filePath.data(), texturePath.data(),
          (int)pointColor[0], (int)pointColor[1], (int)pointColor[2],
          isParallel);
  fclose(fptr);
}
