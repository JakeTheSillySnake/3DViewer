#include <gtest/gtest.h>

#include "../backend/controller.h"

using namespace s21;

TEST(test_Contr, model) {
  Model m;
  Controller c(&m);
  m.saveSettings();
  m.loadSettings();
  QString path = "../../assets/models/cube.obj";
  c.changeFile(path);
  ASSERT_TRUE(m.triangles.size());
}

TEST(test_Contr, vertices) {
  Model m;
  Controller c(&m);
  c.pointColor(0, 0, 0);
  c.pointSize(3);
  c.pointMode(2);
  ASSERT_EQ(m.pointColor[0] + m.pointColor[1] + m.pointColor[2], 0);
  ASSERT_EQ(m.pointSize, 3);
  ASSERT_EQ(m.pointMode, 2);
}

TEST(test_Contr, edges) {
  Model m;
  Controller c(&m);
  c.wireColor(0, 0, 0);
  c.thickness(3);
  ASSERT_EQ(m.wireColor[0] + m.wireColor[1] + m.wireColor[2], 0);
  ASSERT_EQ(m.lineSize, 3);
}

TEST(test_Contr, tex1) {
  Model m;
  Controller c(&m);
  QString path = "default";
  c.openTex(path);
  ASSERT_EQ(m.texPath, "default");
}

TEST(test_Contr, tex2) {
  Model m;
  Controller c(&m);
  c.woodTex();
  ASSERT_EQ(m.texPath, "../../assets/textures/wood.png");
}

TEST(test_Contr, tex3) {
  Model m;
  Controller c(&m);
  c.metalTex();
  ASSERT_EQ(m.texPath, "../../assets/textures/metal.png");
}

TEST(test_Contr, tex4) {
  Model m;
  Controller c(&m);
  c.stoneTex();
  ASSERT_EQ(m.texPath, "../../assets/textures/stone.png");
}

TEST(test_Contr, tex5) {
  Model m;
  Controller c(&m);
  c.waterTex();
  ASSERT_EQ(m.texPath, "../../assets/textures/water.png");
}

TEST(test_Contr, tex6) {
  Model m;
  Controller c(&m);
  c.defaultTex();
  ASSERT_EQ(m.texPath, "../../assets/textures/default.png");
}

TEST(test_Contr, bg) {
  Model m;
  Controller c(&m);
  c.changeBackground(1, 1, 1);
  ASSERT_EQ(m.bgColor[0] + m.bgColor[1] + m.bgColor[2], 3);
}

TEST(test_Contr, rotate) {
  Model m;
  Controller c(&m);
  QQuaternion prev = m.rotation;
  c.rotateModel(1, 0, 0);
  c.rotateModel(0, 1, 0);
  c.rotateModel(0, 0, 1);
  ASSERT_FALSE(m.rotation.isNull());
  ASSERT_TRUE(prev != m.rotation);
}

TEST(test_Contr, scale) {
  Model m;
  Controller c(&m);
  c.scaleModel(1, 0, 0);
  ASSERT_EQ(m.scaleX, 1);
  c.scaleModel(0, 2, 0);
  ASSERT_EQ(m.scaleY, 2);
  c.scaleModel(0, 0, -3);
  ASSERT_EQ(m.scaleZ, -3);
  c.scaleModel(-1, 0, 0);
  ASSERT_EQ(m.scaleX, 0);
  c.scaleModel(0, -2, 0);
  ASSERT_EQ(m.scaleY, 0);
  c.scaleModel(0, 0, 3);
  ASSERT_EQ(m.scaleZ, 0);
}

TEST(test_Contr, translate) {
  Model m;
  Controller c(&m);
  c.translateModel(1, 0, 0);
  ASSERT_EQ(m.translateX, 1);
  c.translateModel(0, 2, 0);
  ASSERT_EQ(m.translateY, 2);
  c.translateModel(0, 0, -3);
  ASSERT_EQ(m.translateZ, -3);
  c.translateModel(-1, 0, 0);
  ASSERT_EQ(m.translateX, 0);
  c.translateModel(0, -2, 0);
  ASSERT_EQ(m.translateY, 0);
  c.translateModel(0, 0, 3);
  ASSERT_EQ(m.translateZ, 0);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
