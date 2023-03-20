#ifndef SCENE_H
#define SCENE_H

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>
#include <memory>

constexpr int INITIAL_WINDOW_WIDTH = 800;
constexpr int INITIAL_WINDOW_HEIGHT = 600;
constexpr std::string_view WINDOW_TITLE = "MAT392 - Mathematics in Computer Graphics";

constexpr std::string_view GLSL_TARGET = "#version 330 core";

class Camera;

struct SceneControls
{
  bool useMouseLock;
  bool useWireFrame;
  bool useSkybox;
};

class Scene
{
public:

  Scene();
  ~Scene();
  void start();

  void framebuffer_size_callback(GLFWwindow* window, int width, int height);
  void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn);
  void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
  int width;
  int height;
  float aspectRatio;
  unsigned int frameCount;

  GLFWwindow* window;

  std::unique_ptr<Camera> camera;

  SceneControls sceneControls;

  void processInput(float deltaTime);

  void initImGui();
  void renderImGui();
  void shutdownImGui();
};

#endif